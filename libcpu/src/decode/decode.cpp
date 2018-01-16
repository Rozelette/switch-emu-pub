#include <array>
#include <vector>

#include "decode.h"
#include "bitutils.h"
#include "unreachable.h"
#include "log.h"
#include "cpu.h"
#include "disassembler.h"

namespace cpu::decoder {

void decodeSystemOrBranch(uint32_t bits, DecodedInstruction& inst) {
    auto op0 = bitRange<29, 31>(bits);
    auto op1 = bitRange<22, 25>(bits);

    switch (op0) {
    case 2:
        // 010
        if (op1 < 8) {
            // 0xxx - Conditional branch (immediate)
            if (testBit<4>(bits) || testBit<24>(bits)) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::B_cond;
            }
        } else {
            // 1xxx - Unallocated
            inst.id = instructionID::INVALID;
        }
        break;
    case 6:
        // 110
        if (op1 > 7) {
            // 1xxx - Unconditional branch (register)
            auto opc = bitRange<21, 24>(bits);
            auto op2 = bitRange<16, 20>(bits);
            auto op3 = bitRange<10, 15>(bits);
            auto Rn = bitRange<5, 9>(bits);
            auto op4 = bitRange<0, 4>(bits);

            if ((op2 != 31) || (op3 != 0) || (op4 != 0)) {
                inst.id = instructionID::INVALID;
                return;
            }

            switch (opc) {
            case 0:
                inst.id = instructionID::BR;
                break;
            case 1:
                inst.id = instructionID::BLR;
                break;
            case 2:
                inst.id = instructionID::RET;
                break;
            case 4:
                if (Rn == 31) {
                    inst.id = instructionID::ERET;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 5:
                if (Rn == 31) {
                    inst.id = instructionID::DRPS;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 3:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:case 14:case 15:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
        } else if (op1 < 4) {
            // 00xx - Exception generation
            decodeExceptionGenerating(bits, inst);
        } else if (op1 == 4) {
            // 0100 - System
            decodeSystem(bits, inst);
        } else {
            // 0101 & 011x - Unallocated
            inst.id = instructionID::INVALID;
        }
        break;
    case 0:case 4:
    {
        // x00 - Unconditional branch (immediate)
        auto isLink = testBit<31>(bits);
        if (isLink) {
            inst.id = instructionID::BL;
        } else {
            inst.id = instructionID::B;
        }
        break;
    }
    case 1:case 5:
        // x01
        if (op1 < 8) {
            // 0xxx - Compare & branch (immediate)
            bool is64 = testBit<31>(bits);
            bool negate = testBit<24>(bits);
            if (is64) {
                if (negate) {
                    inst.id = instructionID::CBNZ_64;
                } else {
                    inst.id = instructionID::CBZ_64;
                }
            } else {
                if (negate) {
                    inst.id = instructionID::CBNZ_32;
                } else {
                    inst.id = instructionID::CBZ_32;
                }
            }
        } else {
            // 1xxx - Test & branch (immediate)
            bool negate = testBit<24>(bits);
            if (negate) {
                inst.id = instructionID::TBNZ;
            } else {
                inst.id = instructionID::TBZ;
            }
        }
        break;
    case 3: case 7:
        // x11 - Unallocated
        inst.id = instructionID::INVALID;
        break;
    default:
        UNREACHABLE();
    }
}

void decodeFloatOrSIMD(uint32_t bits, DecodedInstruction& inst) {
    auto op0 = bitRange<28, 31>(bits);

    switch (op0) {
    case 1:case 3:case 9:case 11:
        // x0x1 - Floating-point
        decodeFloat(bits, inst);
        break;
    case 8:case 10:case 12:case 13:case 14:case 15:
        // 11x1 & 1xx0 - Unallocated
        inst.id = instructionID::INVALID;
        break;
    case 0:case 2:case 4:case 5:case 6:case 7:
        // 0xx0 & 01x1 - SIMD
        decodeSIMD(bits, inst);
        break;
    default:
        UNREACHABLE();
    }
}

void decode(uint32_t bits, DecodedInstruction& inst) {
    inst.inst.bits = bits;
    inst.id = instructionID::INVALID;

    auto opcodeType = bitRange<25, 28>(bits);

    switch (opcodeType) {
    case 0:case 1:case 2:case 3:
        // 00xx - Unallocated
        inst.id = instructionID::INVALID;
        break;
    case 8:case 9:
        // 100x - Data processing - immediate
        decodeDPImm(bits, inst);
        break;
    case 10:case 11:
        // 101x - Branches, exception generating and system instructions
        decodeSystemOrBranch(bits, inst);
        break;
    case 4:case 6:case 12:case 14:
        // x1x0 - Loads and stores
        decodeLoadStore(bits, inst);
        break;
    case 5: case 13:
        // x101 - Data processing - register
        decodeDPReg(bits, inst);
        break;
    case 7:case 15:
        // x111 - Data processing - SIMD and floating point
        decodeFloatOrSIMD(bits, inst);
        break;
    default:
        UNREACHABLE();
    }
}

struct instructionDecodeInfo {
    uint32_t mask = 0xFFFFFFFF;
    uint32_t match = 0;
    std::vector<uint32_t> bitsMustNotAllBeClear;
    std::vector<uint32_t> bitsMustNotAllBeSet;
};

std::array<instructionDecodeInfo, static_cast<size_t>(instructionID::NUM_INSTRUCTIONS)> gInstructionDecodeTable;


void init() {

#define ARG_LIST(...) __VA_ARGS__

#define INST4(x, name, args, mask_, match_, bitsMustNotAllBeClear_, bitsMustNotAllBeSet_) \
gInstructionDecodeTable[static_cast<size_t>(instructionID::x)].mask = mask_; \
gInstructionDecodeTable[static_cast<size_t>(instructionID::x)].match = match_; \
gInstructionDecodeTable[static_cast<size_t>(instructionID::x)].bitsMustNotAllBeClear = { ARG_LIST bitsMustNotAllBeClear_ }; \
gInstructionDecodeTable[static_cast<size_t>(instructionID::x)].bitsMustNotAllBeSet = { ARG_LIST bitsMustNotAllBeSet_ };

#define INST3(x, name, args, mask_, match_, bitsMustNotAllBeClear_) INST4(x, name, args, mask_, match_, bitsMustNotAllBeClear_, ())
#define INST2(x, name, args, mask_, match_) INST4(x, name, args, mask_, match_, (), ())

#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, name, ...) name
#define INST(...) EXPAND(GET_MACRO(__VA_ARGS__, INST4, INST3, INST2)(__VA_ARGS__))

#include "instructions.inl"

#undef INST
#undef GET_MACRO
#undef EXPAND
#undef INST2
#undef INST3
#undef INST4
#undef ARG_LIST
}

void test() {
    uint64_t limit = 0x100000000;
    cpu::instructionID lastError1 = static_cast<cpu::instructionID>(0xFFFFFFFF);
    cpu::instructionID lastError2 = static_cast<cpu::instructionID>(0xFFFFFFFF);
    logger::info("Decode test:");
    for (uint64_t bits = 0; bits < limit; bits++) {
        DecodedInstruction decoded1;
        DecodedInstruction decoded2;

        decode(static_cast<uint32_t>(bits), decoded1);
        decode2(static_cast<uint32_t>(bits), decoded2);

        if ((decoded1.id != decoded2.id) && ((lastError1 != decoded1.id) || (lastError2 != decoded2.id))) {
            lastError1 = decoded1.id;
            lastError2 = decoded2.id;
            std::string name1 = cpu::disassembler::getBaseInstructionName(decoded1.id);
            std::string name2 = cpu::disassembler::getBaseInstructionName(decoded2.id);
            logger::info("%08X: 1:%s, 2:%s", static_cast<uint32_t>(bits), name1.c_str(), name2.c_str());
        }
    }

    cpu::sendInterrupts(0, cpu::DEBUG_BREAK_INTERRUPT);
}

void decode2(uint32_t bits, DecodedInstruction& inst) {
    inst.inst.bits = bits;
    inst.id = instructionID::INVALID;

    for (auto i = 0u; i < gInstructionDecodeTable.size(); i++) {
        const instructionDecodeInfo&info = gInstructionDecodeTable[i];
        if ((bits & info.mask) == info.match) {
            if (info.bitsMustNotAllBeClear.size() != 0) {
                for (auto mask : info.bitsMustNotAllBeClear) {
                    if ((bits & mask) == 0) {
                        goto not_a_match;
                    }
                }
            }
            if (info.bitsMustNotAllBeSet.size() != 0) {
                for (auto mask : info.bitsMustNotAllBeSet) {
                    if ((bits & mask) == mask) {
                        goto not_a_match;
                    }
                }
            }
            inst.id = static_cast<instructionID>(i);
        not_a_match:;
        }
    }
}

} // namespace cpu::decoder