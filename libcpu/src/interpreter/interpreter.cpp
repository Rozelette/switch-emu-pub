#include <array>

#include "interpreter.h"
#include "mem.h"
#include "decode.h"
#include "log.h"
#include "interpreterRegister.h"
#include "disassembler.h"

namespace cpu {

extern IllInstHandler gIllInstHandler;

namespace interpreter {

std::array<interptfptr_t, static_cast<size_t>(instructionID::NUM_INSTRUCTIONS)> gInstructionMap;

void invalidInstructionHandler(Core* core, AArch64Inst inst) {
    logger::error("Invalid instruction: %08X at %016llX", inst.bits, core->PC);
    gIllInstHandler();
}

void init() {
    gInstructionMap.fill(UNIMPLEMENTED_FUNC);

    registerBranchInstructions();
    registerSystemInstructions();
    registerDPImmInstructions();
    registerDPRegInstructions();
    registerFloatInstructions();
    registerLoadStoreInstructions();
    registerSIMDInstructions();

    registerInstruction(instructionID::INVALID, invalidInstructionHandler);
}

void registerInstruction(instructionID id, interptfptr_t func) {
    gInstructionMap[static_cast<size_t>(id)] = func;
}

interptfptr_t getInstructionFunc(instructionID id) {
    return gInstructionMap[static_cast<size_t>(id)];
}

void step(Core* core) {
    auto bits = mem::fetch(core->PC);
    core->nextPC = core->PC + 4;

    DecodedInstruction inst;
    decoder::decode(bits, inst);

    getInstructionFunc(inst.id)(core, inst.inst);

    core->GPRegs[31] = 0; // clear 0 register

    if (core->PC == core->nextPC) {
        logger::error("Infinite loop detected");
        cpu::sendInterrupts(cpu::this_core::getCoreID(), cpu::DEBUG_BREAK_INTERRUPT);
    }

    core->PC = core->nextPC;
    core->numInsts++;
}

void UNIMPLEMENTED_FUNC(cpu::Core* core, cpu::AArch64Inst inst) {
    DecodedInstruction decoded;
    decoder::decode(inst.bits, decoded);
    std::string name = cpu::disassembler::getBaseInstructionName(decoded.id);

    logger::error("Unimplemented instruction %s: 0x%08X at 0x%016llX with id %u", name.c_str(), inst.bits, core->PC, decoded.id);
}

} // namespace interpreter

} // namespace cpu