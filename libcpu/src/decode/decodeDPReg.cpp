#include "decode.h"
#include "bitutils.h"
#include "unreachable.h"
#include "log.h"

namespace cpu::decoder {

void decodeDP2Source(uint32_t bits, DecodedInstruction& inst) {
    bool is64 = testBit<31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto opcode = bitRange<10, 15>(bits);

    if (S == 1) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (opcode) {
    case 2: // 000010 - UDIV
        if (is64) {
            inst.id = instructionID::UDIV_64;
        } else {
            inst.id = instructionID::UDIV_32;
        }
        break;
    case 3: // 000011 - SDIV
        if (is64) {
            inst.id = instructionID::SDIV_64;
        } else {
            inst.id = instructionID::SDIV_32;
        }
        break;
    case 8: // 001000 - LSLV
        if (is64) {
            inst.id = instructionID::LSLV_64;
        } else {
            inst.id = instructionID::LSLV_32;
        }
        break;
    case 9: // 001001 - LSRV
        if (is64) {
            inst.id = instructionID::LSRV_64;
        } else {
            inst.id = instructionID::LSRV_32;
        }
        break;
    case 10: // 001010 - ASRV
        if (is64) {
            inst.id = instructionID::ASRV_64;
        } else {
            inst.id = instructionID::ASRV_32;
        }
        break;
    case 11: // 001011 - RORV
        if (is64) {
            inst.id = instructionID::RORV_64;
        } else {
            inst.id = instructionID::RORV_32;
        }
        break;
    case 16: // 010000 - CRC32B
        if (is64) {
            inst.id = instructionID::INVALID;
        } else {
            inst.id = instructionID::CRC32B;
        }
        break;
    case 17: // 010001 - CRC32H
        if (is64) {
            inst.id = instructionID::INVALID;
        } else {
            inst.id = instructionID::CRC32H;
        }
        break;
    case 18: // 010010 - CRC32W
        if (is64) {
            inst.id = instructionID::INVALID;
        } else {
            inst.id = instructionID::CRC32W;
        }
        break;
    case 19: // 010011 - CRC32X
        if (is64) {
            inst.id = instructionID::CRC32X;
        } else {
            inst.id = instructionID::INVALID;
        }
        break;
    case 20: // 010100 - CRC32CB
        if (is64) {
            inst.id = instructionID::INVALID;
        } else {
            inst.id = instructionID::CRC32CB;
        }
        break;
    case 21: // 010101 - CRC32CH
        if (is64) {
            inst.id = instructionID::INVALID;
        } else {
            inst.id = instructionID::CRC32CH;
        }
        break;
    case 22: // 010110 - CRC32CW
        if (is64) {
            inst.id = instructionID::INVALID;
        } else {
            inst.id = instructionID::CRC32CW;
        }
        break;
    case 23: // 010111 - CRC32CX
        if (is64) {
            inst.id = instructionID::CRC32CX;
        } else {
            inst.id = instructionID::INVALID;
        }
        break;
    default:
        inst.id = instructionID::INVALID;
    }
}

void decodeDP1Source(uint32_t bits, DecodedInstruction& inst) {
    bool is64 = testBit<31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto opcode2 = bitRange<16, 20>(bits);
    auto opcode = bitRange<10, 15>(bits);

    if (opcode > 5) {
        // 1xxxxx & x1xxxx & xx1xxx & 00011x - Unallocated
        inst.id = instructionID::INVALID;
        return;
    }
    if (opcode2 != 0) {
        inst.id = instructionID::INVALID;
        return;
    }
    if (S == 1) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (opcode) {
    case 0: // 000000 - RBIT
        if (is64) {
            inst.id = instructionID::RBIT_64;
        } else {
            inst.id = instructionID::RBIT_32;
        }
        break;
    case 1: // 000001 - REV16
        if (is64) {
            inst.id = instructionID::REV16_64;
        } else {
            inst.id = instructionID::REV16_32;
        }
        break;
    case 2: // 000010 - REV
        if (is64) {
            inst.id = instructionID::REV32_64;
        } else {
            inst.id = instructionID::REV_32;
        }
        break;
    case 3: // 000011 - REV32
        if (is64) {
            inst.id = instructionID::REV_64;
        } else {
            inst.id = instructionID::INVALID;
        }
        break;
    case 4: // 000100 - CLZ
        if (is64) {
            inst.id = instructionID::CLZ_64;
        } else {
            inst.id = instructionID::CLZ_32;
        }
        break;
    case 5: // 000101 - CLS
        if (is64) {
            inst.id = instructionID::CLS_64;
        } else {
            inst.id = instructionID::CLS_32;
        }
        break;
    }
}

void decodeLogicalReg(uint32_t bits, DecodedInstruction& inst) {
    bool is64 = testBit<31>(bits);
    auto opc = bitRange<29, 30>(bits);
    bool negate = testBit<21>(bits);
    auto imm = bitRange<10, 15>(bits);
    if (!is64 && (imm > 31)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (is64) {
        switch (opc) {
        case 0:
            // AND (shifted register)
            if (negate) {
                inst.id = instructionID::BIC_64_shifted;
            } else {
                inst.id = instructionID::AND_64_shifted;
            }
            break;
        case 1:
            // ORR (shifted register)
            if (negate) {
                inst.id = instructionID::ORN_64_shifted;
            } else {
                inst.id = instructionID::ORR_64_shifted;
            }
            break;
        case 2:
            // EOR (shifted register)
            if (negate) {
                inst.id = instructionID::EON_64_shifted;
            } else {
                inst.id = instructionID::EOR_64_shifted;
            }
            break;
        case 3:
            // ANDS (shifted register)
            if (negate) {
                inst.id = instructionID::BICS_64_shifted;
            } else {
                inst.id = instructionID::ANDS_64_shifted;
            }
            break;
        }
    } else {
        switch (opc) {
        case 0:
            // AND (shifted register)
            if (negate) {
                inst.id = instructionID::BIC_32_shifted;
            } else {
                inst.id = instructionID::AND_32_shifted;
            }
            break;
        case 1:
            // ORR (shifted register)
            if (negate) {
                inst.id = instructionID::ORN_32_shifted;
            } else {
                inst.id = instructionID::ORR_32_shifted;
            }
            break;
        case 2:
            // EOR (shifted register)
            if (negate) {
                inst.id = instructionID::EON_32_shifted;
            } else {
                inst.id = instructionID::EOR_32_shifted;
            }
            break;
        case 3:
            // ANDS (shifted register)
            if (negate) {
                inst.id = instructionID::BICS_32_shifted;
            } else {
                inst.id = instructionID::ANDS_32_shifted;
            }
            break;
        }
    }
}

void decodeAddSubShifted(uint32_t bits, DecodedInstruction& inst) {
    // b31 - is64
    // b30 - isSub
    // b29 - updatesFlags
    auto flags = bitRange<29, 31>(bits);
    auto shift = bitRange<22, 23>(bits);

    if (shift == 3) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (testBit<15>(bits) && !testBit<31>(bits)) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (flags) {
    case 0:
        // 000
        inst.id = instructionID::ADD_32_shifted;
        break;
    case 1:
        // 001
        inst.id = instructionID::ADDS_32_shifted;
        break;
    case 2:
        // 010
        inst.id = instructionID::SUB_32_shifted;
        break;
    case 3:
        // 011
        inst.id = instructionID::SUBS_32_shifted;
        break;
    case 4:
        // 100
        inst.id = instructionID::ADD_64_shifted;
        break;
    case 5:
        // 101
        inst.id = instructionID::ADDS_64_shifted;
        break;
    case 6:
        // 110
        inst.id = instructionID::SUB_64_shifted;
        break;
    case 7:
        // 111
        inst.id = instructionID::SUBS_64_shifted;
        break;
    }
}

void decodeAddSubExtended(uint32_t bits, DecodedInstruction& inst) {
    // b31 - is64
    // b30 - isSub
    // b29 - updatesFlags
    auto flags = bitRange<29, 31>(bits);
    auto opt = bitRange<22, 23>(bits);
    auto imm = bitRange<10, 12>(bits);

    if (opt != 0) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (imm > 4) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (flags) {
    case 0:
        // 000
        inst.id = instructionID::ADD_32_extended;
        break;
    case 1:
        // 001
        inst.id = instructionID::ADDS_32_extended;
        break;
    case 2:
        // 010
        inst.id = instructionID::SUB_32_extended;
        break;
    case 3:
        // 011
        inst.id = instructionID::SUBS_32_extended;
        break;
    case 4:
        // 100
        inst.id = instructionID::ADD_64_extended;
        break;
    case 5:
        // 101
        inst.id = instructionID::ADDS_64_extended;
        break;
    case 6:
        // 110
        inst.id = instructionID::SUB_64_extended;
        break;
    case 7:
        // 111
        inst.id = instructionID::SUBS_64_extended;
        break;
    }
}

void decodeAddSubCarry(uint32_t bits, DecodedInstruction& inst) {
    // b31 - is64
    // b30 - isSub
    // b29 - updatesFlags
    auto flags = bitRange<29, 31>(bits);
    auto opcode2 = bitRange<10, 15>(bits);

    if (opcode2 != 0) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (flags) {
    case 0:
        // 000
        inst.id = instructionID::ADC_32;
        break;
    case 1:
        // 001
        inst.id = instructionID::ADCS_32;
        break;
    case 2:
        // 010
        inst.id = instructionID::SBC_32;
        break;
    case 3:
        // 011
        inst.id = instructionID::SBCS_32;
        break;
    case 4:
        // 100
        inst.id = instructionID::ADC_64;
        break;
    case 5:
        // 101
        inst.id = instructionID::ADCS_64;
        break;
    case 6:
        // 110
        inst.id = instructionID::SBC_64;
        break;
    case 7:
        // 111
        inst.id = instructionID::SBCS_64;
        break;
    }
}

void decodeCondCmpReg(uint32_t bits, DecodedInstruction& inst) {
    bool is64 = testBit<31>(bits);
    bool negate = testBit<30>(bits);
    auto S = bitRange<29, 29>(bits);
    auto o2 = bitRange<10, 10>(bits);
    auto o3 = bitRange<4, 4>(bits);

    if ((o3 == 1) || (o2 == 1) || (S == 0)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (is64) {
        if (negate) {
            inst.id = instructionID::CCMP_64_reg;
        } else {
            inst.id = instructionID::CCMN_64_reg;
        }
    } else {
        if (negate) {
            inst.id = instructionID::CCMP_32_reg;
        } else {
            inst.id = instructionID::CCMN_32_reg;
        }
    }
}

void decodeCondCmpImm(uint32_t bits, DecodedInstruction& inst) {
    bool is64 = testBit<31>(bits);
    bool negate = testBit<30>(bits);
    auto S = bitRange<29, 29>(bits);
    auto o2 = bitRange<10, 10>(bits);
    auto o3 = bitRange<4, 4>(bits);

    if ((o3 == 1) || (o2 == 1) || (S == 0)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (is64) {
        if (negate) {
            inst.id = instructionID::CCMP_64_imm;
        } else {
            inst.id = instructionID::CCMN_64_imm;
        }
    } else {
        if (negate) {
            inst.id = instructionID::CCMP_32_imm;
        } else {
            inst.id = instructionID::CCMN_32_imm;
        }
    }
}

void decodeCondSel(uint32_t bits, DecodedInstruction& inst) {
    bool is64 = testBit<31>(bits);
    bool invert = testBit<30>(bits);
    auto S = bitRange<29, 29>(bits);
    auto op2 = bitRange<10, 11>(bits);

    if ((S == 1) || (op2 > 1)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (op2 == 1) {
        // op2 indicates we are incrementing
        if (is64) {
            if (invert) {
                inst.id = instructionID::CSNEG_64;
            } else {
                inst.id = instructionID::CSINC_64;
            }
        } else {
            if (invert) {
                inst.id = instructionID::CSNEG_32;
            } else {
                inst.id = instructionID::CSINC_32;
            }
        }
    } else {
        // op2 == 0
        if (is64) {
            if (invert) {
                inst.id = instructionID::CSINV_64;
            } else {
                inst.id = instructionID::CSEL_64;
            }
        } else {
            if (invert) {
                inst.id = instructionID::CSINV_32;
            } else {
                inst.id = instructionID::CSEL_32;
            }
        }
    }
}

void decodeDP3Source(uint32_t bits, DecodedInstruction& inst) {
    bool is64 = testBit<31>(bits);
    auto op54 = bitRange<29, 30>(bits);
    auto op31 = bitRange<21, 23>(bits);
    auto o0 = bitRange<15, 15>(bits);

    if (op54 != 0) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (is64) {
        switch (op31) {
        case 0:
            // 000 - MADD/MSUB
            if (o0) {
                inst.id = instructionID::MSUB_64;
            } else {
                inst.id = instructionID::MADD_64;
            }
            break;
        case 1:
            // 001 - SMADDL/SMSUBL
            if (o0) {
                inst.id = instructionID::SMSUBL;
            } else {
                inst.id = instructionID::SMADDL;
            }
            break;
        case 2:
            // 010 - SMULH
            if (o0 == 1) {
                inst.id = instructionID::INVALID;
                return;
            }

            inst.id = instructionID::SMULH;
            break;
        case 5:
            // 101 - UMADDL/UMSUBL
            if (o0) {
                inst.id = instructionID::UMSUBL;
            } else {
                inst.id = instructionID::UMADDL;
            }
            break;
        case 6:
            // 110 - UMULH
            if (o0 == 1) {
                inst.id = instructionID::INVALID;
                return;
            }

            inst.id = instructionID::UMULH;
            break;
        case 3:case 4:case 7:
            // 011 & 100 & 111 - Unallocated
            inst.id = instructionID::INVALID;
            break;
        }
    } else {
        // is64 == false
        if (op31 != 0) {
            inst.id = instructionID::INVALID;
            return;
        }

        if (o0) {
            inst.id = instructionID::MSUB_32;
        } else {
            inst.id = instructionID::MADD_32;
        }
    }
}

void decodeDPReg(uint32_t bits, DecodedInstruction& inst) {
    auto op0 = bitRange<30, 30>(bits);
    auto op1 = bitRange<28, 28>(bits);
    auto op2 = bitRange<21, 24>(bits);
    auto op3 = bitRange<11, 11>(bits);

    if (op1 == 1) {
        switch (op2) {
        case 0:
        {
            // 0000 - Add/subtract (with carry)
            decodeAddSubCarry(bits, inst);
            break;
        }
        case 2:
            // 0010 - Conditional compare
            if (op3 == 0) {
                // Conditional compare register
                decodeCondCmpReg(bits, inst);
            } else {
                //op3 == 1
                // Conditional compare immediate
                decodeCondCmpImm(bits, inst);
            }
            break;
        case 4:
        {
            // 0100 - Conditional select
            decodeCondSel(bits, inst);
            break;
        }
        case 6:
            // 0110
            if (op0 == 0) {
                // Data-processing (2 source)
                decodeDP2Source(bits, inst);
            } else {
                // op0 == 1
                // Data-processing (1 source)
                decodeDP1Source(bits, inst);
            }
            break;
        case 8:case 9:case 10:case 11:case 12:case 13:case 14: case 15:
        {
            // 1xxx - Data-processing (3 source)
            decodeDP3Source(bits, inst);
            break;
        }
        case 1:case 3:case 5:case 7:
            // 0xx1 - Unallocated
            inst.id = instructionID::INVALID;
            break;
        }
    } else {
        // op1 == 0
        switch (op2) {
        case 0:case 1: case 2:case 3:case 4:case 5:case 6:case 7:
        {
            // 0xxx - Logical (shifted register)
            decodeLogicalReg(bits, inst);
            break;
        }
        case 8:case 10:case 12: case 14:
        {
            // 1xx0 - Add/subtract (shifted register)
            decodeAddSubShifted(bits, inst);
            break;
        }
        case 9:case 11:case 13:case 15:
        {
            // 1xx1 - Add/subtract (extended register)
            decodeAddSubExtended(bits, inst);
            break;
        }
        default:
            inst.id = instructionID::INVALID;
        }
    }
}

} // namespace cpu::decoder