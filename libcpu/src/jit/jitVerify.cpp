#include <cstring>
#include <mutex>

#include "jitVerify.h"
#include "decode.h"
#include "interpreter\interpreter.h"
#include "emu_assert.h"
#include "log.h"
#include "disassembler.h"

namespace cpu {

namespace jit {

std::mutex gMemoryLock;

bool doVerify(instructionID id) {
    return !(((id >= instructionID::SVC) && (id <= instructionID::DCPS3)) || // don't verify exception-generating
        ((id >= instructionID::MSR_imm) && (id <= instructionID::MRS))); // don't verify system
        //((id >= instructionID::STXRB) && (id <= instructionID::LDR64_imm_float)));  // don't verify load-store (for now)
}

bool isLoadStore(instructionID id) {
    return ((id >= instructionID::STXRB) && (id <= instructionID::LDR64_imm_float));
}

void verifyPreInst(Core* core, VerifyInfo* info, AArch64Inst inst) {
    std::memcpy(&info->savedState, core, sizeof(cpu::Core));

    DecodedInstruction dInst;
    decoder::decode(inst.bits, dInst);
    if (isLoadStore(dInst.id)) {
        gMemoryLock.lock();
    }
}

// TODO check mem write locations, data
void verifyPostInst(Core* core, VerifyInfo* info, AArch64Inst inst) {
    //printf("Doing verify\n");
    DecodedInstruction dInst;
    decoder::decode(inst.bits, dInst);
    info->savedState.nextPC = info->savedState.PC + 4;

    auto fptr = interpreter::getInstructionFunc(dInst.id);

    // TODO errors
    if (fptr) {
        fptr(&info->savedState, dInst.inst);
    }

    info->savedState.GPRegs[31] = 0;

    if (isLoadStore(dInst.id)) {
        gMemoryLock.unlock();
    }

    if (core->nextPC != info->savedState.nextPC) {
        logger::error("\nJit verify: Wrong nextPC at %016llX: %08X %s\nFound %016llX\nExpected: %016llX",
            core->PC,
            inst.bits,
            cpu::disassembler::getBaseInstructionName(dInst.id).c_str(),
            core->nextPC,
            info->savedState.nextPC);
        //emu_abort("Failed jit verify");
    }

    if (core->SP != info->savedState.SP) {
        logger::error("\nJit verify: Wrong SP at %016llX: %08X %s\nFound %016llX\nExpected: %016llX",
            core->PC,
            inst.bits,
            cpu::disassembler::getBaseInstructionName(dInst.id).c_str(),
            core->SP,
            info->savedState.SP);
        //emu_abort("Failed jit verify");
    }

    for (int i = 0; i < 32; i++) {
        if (core->GPRegs[i] != info->savedState.GPRegs[i]) {
            logger::error("\nJit verify: Wrong X%d at %016llX: %08X %s\nFound %016llX\nExpected: %016llX",
                i,
                core->PC,
                inst.bits,
                cpu::disassembler::getBaseInstructionName(dInst.id).c_str(),
                core->GPRegs[i],
                info->savedState.GPRegs[i]);
            //emu_abort("Failed jit verify");
        }
    }

    if (core->PSTATE.N != info->savedState.PSTATE.N) {
        logger::error("\nJit verify: Wrong PSTATE.N at %016llX: %08X %s\nFound %02X\nExpected: %02X",
            core->PC,
            inst.bits,
            cpu::disassembler::getBaseInstructionName(dInst.id).c_str(),
            core->PSTATE.N,
            info->savedState.PSTATE.N);
        //emu_abort("Failed jit verify");
    }

    if (core->PSTATE.Z != info->savedState.PSTATE.Z) {
        logger::error("\nJit verify: Wrong PSTATE.Z at %016llX: %08X %s\nFound %02X\nExpected: %02X",
            core->PC,
            inst.bits,
            cpu::disassembler::getBaseInstructionName(dInst.id).c_str(),
            core->PSTATE.Z,
            info->savedState.PSTATE.Z);
        //emu_abort("Failed jit verify");
    }

    if (core->PSTATE.C != info->savedState.PSTATE.C) {
        logger::error("\nJit verify: Wrong PSTATE.C at %016llX: %08X %s\nFound %02X\nExpected: %02X",
            core->PC,
            inst.bits,
            cpu::disassembler::getBaseInstructionName(dInst.id).c_str(),
            core->PSTATE.C,
            info->savedState.PSTATE.C);
        //emu_abort("Failed jit verify");
    }

    if (core->PSTATE.V != info->savedState.PSTATE.V) {
        logger::error("\nJit verify: Wrong PSTATE.V at %016llX: %08X %s\nFound %02X\nExpected: %02X",
            core->PC,
            inst.bits,
            cpu::disassembler::getBaseInstructionName(dInst.id).c_str(),
            core->PSTATE.V,
            info->savedState.PSTATE.V);
        //emu_abort("Failed jit verify");
    }

    // TODO VRegs, PSTATE, others
}

} // namespace jit

} // namespace cpu