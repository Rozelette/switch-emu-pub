#include <map>
#include <vector>

#include "jit.h"
#include "cpu.h"
#include "interpreter\interpreter.h"
#include "log.h"
#include "mem.h"
#include "decode.h"
#include "meta.h"
#include "bitutils.h"
#include "jitRegister.h"
#include "jitVerify.h"
#include "jitStats.h"

namespace cpu::jit {

std::vector<jitfptr_t> gInstructionMap;

std::map<uint64_t, JitCode> gBlockMap;

asmjit::Runtime* gRunTime;
JitEntryFunc gBlockEntry;
JitFinaleFunc gBlockFinale;

bool gVerify = false;
// TODO thread-safe
VerifyInfo gVerifyInfo;

std::array<uint64_t, static_cast<size_t>(instructionID::NUM_INSTRUCTIONS)> gFallbacks;

void FALLBACK(AArch64EmuAssembler& a, AArch64Inst inst) {
    DecodedInstruction dInst;
    decoder::decode(inst.bits, dInst);

    a.envictAll();

    write64ImmToMem(a, a.corePCMem, a.currentPC);
    write64ImmToMem(a, a.coreNextPCMem, a.currentPC + 4);

    auto interFunc = interpreter::getInstructionFunc(dInst.id);
    a.mov(a.argRegs[0], a.stateReg);
    a.mov(a.argRegs[1], inst.bits);
    a.call(asmjit::imm_ptr(interFunc));

    auto temp = a.allocGpTemp(8);

    a.mov(temp, reinterpret_cast<uint64_t>(&gFallbacks[static_cast<size_t>(dInst.id)]));
    asmjit::X86Mem fallbackCounter = asmjit::x86::ptr(temp);
    a.inc(fallbackCounter);
}

void init() {
    gInstructionMap.resize(static_cast<size_t>(instructionID::NUM_INSTRUCTIONS));
    registerBranchInstructions();
    registerSystemInstructions();
    registerDPImmInstructions();
    registerDPRegInstructions();
    registerFloatInstructions();
    registerLoadStoreInstructions();

    gRunTime = new asmjit::JitRuntime();

    asmjit::CodeHolder code;
    code.init(gRunTime->getCodeInfo());

    AArch64EmuAssembler a(&code);

    auto entryLabel = a.newLabel();
    auto exitLabel = a.newLabel();

    // create a function of the form void(*)(Core* core, void(*func)())
    // where core is the state of the machine and func is the jitted block of code

    // currently 20 + 8 bytes from the incoming CALL + even amount of pushes
    // needs to be keep 16 byte aligned
    uint64_t stackSize = 0x28; // TODO find a good value

    // TODO assumes Windows calling convention
    a.bind(entryLabel);
    a.push(asmjit::x86::rbp);
    a.push(asmjit::x86::rbx);
    a.push(asmjit::x86::rdi);
    a.push(asmjit::x86::rsi);
    a.push(asmjit::x86::r12);
    a.push(asmjit::x86::r13);
    a.push(asmjit::x86::r14);
    a.push(asmjit::x86::r15);
    a.sub(asmjit::x86::rsp, stackSize);

    a.mov(a.stateReg, a.argRegs[0]);

    a.jmp(a.argRegs[1]);

    a.bind(exitLabel);

    a.add(asmjit::x86::rsp, stackSize);
    a.pop(asmjit::x86::r15);
    a.pop(asmjit::x86::r14);
    a.pop(asmjit::x86::r13);
    a.pop(asmjit::x86::r12);
    a.pop(asmjit::x86::rsi);
    a.pop(asmjit::x86::rdi);
    a.pop(asmjit::x86::rbx);
    a.pop(asmjit::x86::rbp);
    a.ret();

    uint8_t* funcBase = nullptr;
    gRunTime->add(&funcBase, &code);

    gBlockEntry = reinterpret_cast<JitEntryFunc>(funcBase + code.getLabelOffset(entryLabel));
    gBlockFinale = reinterpret_cast<JitFinaleFunc>(funcBase + code.getLabelOffset(exitLabel));
}

void registerInstruction(instructionID id, jitfptr_t func) {
    gInstructionMap[static_cast<size_t>(id)] = func;
}

bool generate(JitBlock& block) {
    asmjit::CodeHolder code;
    code.init(gRunTime->getCodeInfo());

    AArch64EmuAssembler a(&code);

    a.currentPC = block.start;

    // marker
    for (int i = 0; i < 12; i++) {
        a.nop();
    }
    //a.int3();

    uint64_t size = (block.end - block.start) >> 2;
    asmjit::X86Mem instCounter(a.stateReg, static_cast<int32_t>(offsetof(Core, numInsts)));
    auto reg = a.allocGpTemp(8);
    a.mov(reg, instCounter);
    a.add(reg, size);
    a.mov(instCounter, reg);

    for (auto addr = block.start; addr < block.end; addr += 4) {
        a.nop(); // marker
        auto bits = mem::fetch(addr);

        // TODO avoid decoding this so many times?
        DecodedInstruction inst;
        decoder::decode(bits, inst);

        jitfptr_t fptr = nullptr;
        if (inst.id != instructionID::INVALID) {
            fptr = gInstructionMap[static_cast<size_t>(inst.id)];
        } else {
            logger::error("Invalid instruction attempted to be jitted");
        }

        // TODO rewrite allocators to remove this
        write64ImmToMem(a, a.coreGPRegs[31], 0);

        bool verifing = gVerify & doVerify(inst.id);

        if (verifing) {
            write64ImmToMem(a, a.corePCMem, a.currentPC);
            write64ImmToMem(a, a.coreNextPCMem, a.currentPC + 4);

            a.mov(a.argRegs[0], a.stateReg);
            a.mov(a.argRegs[1], asmjit::imm_ptr(&gVerifyInfo));
            a.mov(a.argRegs[2], bits);
            a.call(asmjit::imm_ptr(verifyPreInst));
        }

        if (fptr) {
            fptr(a, inst.inst);
        } else {
            logger::error("Missing jit function for id: %d", inst.id);
            FALLBACK(a, inst.inst);
        }

        if (verifing) {
            a.envictAll();

            a.mov(a.argRegs[0], a.stateReg);
            a.mov(a.argRegs[1], asmjit::imm_ptr(&gVerifyInfo));
            a.mov(a.argRegs[2], bits);
            a.call(asmjit::imm_ptr(verifyPostInst));
        }

        a.currentPC += 4;
    }

    a.envictAll();

    a.jmp(asmjit::imm_ptr(gBlockFinale));

    code.sync();

    auto err = gRunTime->add(&block.funcEntry, &code);
    if (err) {
        logger::error("Could not bind jit function");
    }
    return true;
}

bool findBlockEnd(JitBlock& block) {
    uint64_t currentAddr = block.start;
    // TODO impose a MAX_INST limit so this function is guaranteed to terminate
    do {
        auto bits = mem::fetch(currentAddr);
        uint64_t nextAddr = currentAddr + 4;

        DecodedInstruction inst;
        decoder::decode(bits, inst);

        if (cpu::meta::isBranch(inst.id)) {
            block.end = nextAddr;
        }

        currentAddr = nextAddr;
    } while ((block.end == block.start) && (((currentAddr - block.start) >> 2) < 3000));

    logger::info("Found block from %016llX to %016llX", block.start, block.end);
    return true;
}

JitCode get(uint64_t addr) {
    auto findBlock = gBlockMap.find(addr);
    if (findBlock != gBlockMap.end()) {
        // function has already been generated, so let's return it
        return findBlock->second;
    } else {
        JitBlock block(addr);

        if (!findBlockEnd(block)) {
            return nullptr;
        }

        if (!generate(block)) {
            return nullptr;
        }

        gBlockMap.emplace(addr, block.funcEntry);

        return block.funcEntry;
    }
}

void step(Core* core) {
    auto func = get(core->PC);

    executeCode(core, func);

    core->PC = core->nextPC;
}

void executeCode(Core* core, JitCode func) {
    gBlockEntry(core, func);
}

uint64_t getFallbackCount(instructionID id) {
    return gFallbacks[static_cast<size_t>(id)];
}

void write64ImmToMem(AArch64EmuAssembler& a, asmjit::X86Mem& mem, uint64_t data) {
    asmjit::X86Mem lower = asmjit::X86Mem(mem);
    lower.setSize(4);
    asmjit::X86Mem upper = asmjit::X86Mem(lower);
    upper.addOffset(4);
    a.mov(lower, static_cast<uint32_t>(data));
    a.mov(upper, static_cast<uint32_t>(data >> 32));
}

} // namespace cpu::jit