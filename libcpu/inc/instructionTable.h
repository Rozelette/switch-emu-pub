#pragma once

namespace cpu {

#define INST(x, ...) x,
enum class instructionID : uint32_t {
    INVALID,
#include "instructions.inl"
    NUM_INSTRUCTIONS,
};
#undef INST

} // namespace cpu