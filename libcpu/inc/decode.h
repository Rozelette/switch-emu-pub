#pragma once
#include <stdint.h>

#include "instruction.h"

namespace cpu {

struct DecodedInstruction {
    DecodedInstruction() : id(instructionID::INVALID) {}

    instructionID id;
    AArch64Inst inst;
};

namespace decoder {

void init();

void decode(uint32_t bits, DecodedInstruction& inst);
void decode2(uint32_t bits, DecodedInstruction& inst);

void decodeFloat(uint32_t bits, DecodedInstruction& inst);
void decodeLoadStore(uint32_t bits, DecodedInstruction& inst);
void decodeSystem(uint32_t bits, DecodedInstruction& inst);
void decodeExceptionGenerating(uint32_t bits, DecodedInstruction& inst);
void decodeDPImm(uint32_t bits, DecodedInstruction& inst);
void decodeDPReg(uint32_t bits, DecodedInstruction& inst);
void decodeSIMD(uint32_t bits, DecodedInstruction& inst);

void test();

} // namespace decoder

} // namespace cpu
