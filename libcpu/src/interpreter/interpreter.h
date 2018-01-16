#pragma once
#include "cpu.h"
#include "log.h"
#include "instruction.h"

namespace cpu {

namespace interpreter {

using interptfptr_t = void(*)(Core*, AArch64Inst);

void init();
void registerInstruction(instructionID id, interptfptr_t func);
interptfptr_t getInstructionFunc(instructionID id);

void step(Core* core);

void UNIMPLEMENTED_FUNC(cpu::Core* core, cpu::AArch64Inst inst);

inline void nop(cpu::Core* core, cpu::AArch64Inst inst) {}

} // namespace interpreter

} // namespace cpu
