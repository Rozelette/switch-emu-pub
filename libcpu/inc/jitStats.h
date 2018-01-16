#pragma once

#include "cpu.h"
#include "instruction.h"

namespace cpu::jit{

uint64_t getFallbackCount(instructionID id);

} // namespace cpu::jit