#include "decode.h"
#include "bitutils.h"
#include "unreachable.h"
#include "log.h"

namespace cpu::decoder {

void decodeConvertFloatAndFixed(uint32_t bits, DecodedInstruction& inst) {
    auto sf = bitRange<31, 31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto type = bitRange<22, 23>(bits);
    auto rmode = bitRange<19, 20>(bits);
    auto opcode = bitRange<16, 18>(bits);

    if (S == 1) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (sf == 0) {
        if (type == 0) {
            if ((rmode == 0) && (opcode == 2)) {
                inst.id = instructionID::SCVTF_32_single_fixed;
            } else if ((rmode == 0) && (opcode == 3)) {
                inst.id = instructionID::UCVTF_32_single_fixed;
            } else if((rmode == 3) && (opcode == 0)) {
                inst.id = instructionID::FCVTZS_single_32_fixed;
            } else if((rmode == 3) && (opcode == 1)) {
                inst.id = instructionID::FCVTZU_single_32_fixed;
            } else {
                inst.id = instructionID::INVALID;
                return;
            }
        } else if (type == 1) {
            if ((rmode == 0) && (opcode == 2)) {
                inst.id = instructionID::SCVTF_32_double_fixed;
            } else if ((rmode == 0) && (opcode == 3)) {
                inst.id = instructionID::UCVTF_32_double_fixed;
            } else if ((rmode == 3) && (opcode == 0)) {
                inst.id = instructionID::FCVTZS_double_32_fixed;
            } else if ((rmode == 3) && (opcode == 1)) {
                inst.id = instructionID::FCVTZU_double_32_fixed;
            } else {
                inst.id = instructionID::INVALID;
                return;
            }
        } else {
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        if (type == 0) {
            if ((rmode == 0) && (opcode == 2)) {
                inst.id = instructionID::SCVTF_64_single_fixed;
            } else if ((rmode == 0) && (opcode == 3)) {
                inst.id = instructionID::UCVTF_64_single_fixed;
            } else if ((rmode == 3) && (opcode == 0)) {
                inst.id = instructionID::FCVTZS_single_64_fixed;
            } else if ((rmode == 3) && (opcode == 1)) {
                inst.id = instructionID::FCVTZU_single_64_fixed;
            } else {
                inst.id = instructionID::INVALID;
                return;
            }
        } else if (type == 1) {
            if ((rmode == 0) && (opcode == 2)) {
                inst.id = instructionID::SCVTF_64_double_fixed;
            } else if ((rmode == 0) && (opcode == 3)) {
                inst.id = instructionID::UCVTF_64_double_fixed;
            } else if ((rmode == 3) && (opcode == 0)) {
                inst.id = instructionID::FCVTZS_double_64_fixed;
            } else if ((rmode == 3) && (opcode == 1)) {
                inst.id = instructionID::FCVTZU_double_64_fixed;
            } else {
                inst.id = instructionID::INVALID;
                return;
            }
        } else {
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeConvertFloatAndInt(uint32_t bits, DecodedInstruction& inst) {
    auto sf = bitRange<31, 31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto type = bitRange<22, 23>(bits);
    auto rmode = bitRange<19, 20>(bits);
    auto opcode = bitRange<16, 18>(bits);

    if (S == 1) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (sf == 0) {
        if (type == 0) {
            switch (rmode) {
            case 0:
                switch (opcode) {
                case 0:
                    // 000 - FCVTNS (scalar) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTNS_single_32_int;
                    break;
                case 1:
                    // 001- FCVTNU (scalar) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTNU_single_32_int;
                    break;
                case 2:
                    // 010 - SCVTF (scalar, integer) - 32-bit to single-precision
                    inst.id = instructionID::SCVTF_32_single_int;
                    break;
                case 3:
                    // 011 - UCVTF (scalar, integer) - 32-bit to single-precision
                    inst.id = instructionID::UCVTF_32_single_int;
                    break;
                case 4:
                    // 100 - FCVTAS (scalar) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTAS_single_32_int;
                    break;
                case 5:
                    // 101 - FCVTAU (scalar) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTAU_single_32_int;
                    break;
                case 6:
                    // 110- FMOV (general) - Single-precision to 32-bit
                    inst.id = instructionID::FMOV_single_32;
                    break;
                case 7:
                    // 111- FMOV (general) - 32-bit to single-precision
                    inst.id = instructionID::FMOV_32_single;
                    break;
                default:
                    UNREACHABLE();
                }
                break;
            case 1:
                if (opcode == 0) {
                    // FCVTPS (scalar) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTPS_single_32_int;
                } else if (opcode == 1) {
                    // FCVTPU (scalar) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTPU_single_32_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 2:
                if (opcode == 0) {
                    // FCVTMS (scalar) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTMS_single_32_int;
                } else if (opcode == 1) {
                    // FCVTMU (scalar) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTMU_single_32_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 3:
                if (opcode == 0) {
                    // FCVTZS (scalar, integer) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTZS_single_32_int;
                } else if (opcode == 1) {
                    // FCVTZU (scalar, integer) - Single-precision to 32-bit
                    inst.id = instructionID::FCVTZU_single_32_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            default:
                UNREACHABLE();
            }
        } else if (type == 1) {
            switch (rmode) {
            case 0:
                switch (opcode) {
                case 0:
                    // 000 - FCVTNS (scalar) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTNS_double_32_int;
                    break;
                case 1:
                    // 001- FCVTNU (scalar) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTNU_double_32_int;
                    break;
                case 2:
                    // 010 - SCVTF (scalar, integer) - 32-bit to double-precision
                    inst.id = instructionID::SCVTF_32_double_int;
                    break;
                case 3:
                    // 011 - UCVTF (scalar, integer) - 32-bit to double-precision
                    inst.id = instructionID::UCVTF_32_double_int;
                    break;
                case 4:
                    // 100 - FCVTAS (scalar) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTAS_double_32_int;
                    break;
                case 5:
                    // 101 - FCVTAU (scalar) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTAU_double_32_int;
                    break;
                case 6:case 7:
                    inst.id = instructionID::INVALID;
                    break;
                default:
                    UNREACHABLE();
                }
                break;
            case 1:
                if (opcode == 0) {
                    // FCVTPS (scalar) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTPS_double_32_int;
                } else if (opcode == 1) {
                    // FCVTPU (scalar) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTPU_double_32_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 2:
                if (opcode == 0) {
                    // FCVTMS (scalar) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTMS_double_32_int;
                } else if (opcode == 1) {
                    // FCVTMU (scalar) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTMU_double_32_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 3:
                if (opcode == 0) {
                    // FCVTZS (scalar, integer) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTZS_double_32_int;
                } else if (opcode == 1) {
                    // FCVTZU (scalar, integer) - Double-precision to 32-bit
                    inst.id = instructionID::FCVTZU_double_32_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            default:
                UNREACHABLE();
            }
        } else {
            inst.id = instructionID::INVALID;
        }
    } else {
        // sf == 1
        if (type == 0) {
            switch (rmode) {
            case 0:
                switch (opcode) {
                case 0:
                    // 000 - FCVTNS (scalar) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTNS_single_64_int;
                    break;
                case 1:
                    // 001- FCVTNU (scalar) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTNU_single_64_int;
                    break;
                case 2:
                    // 010 - SCVTF (scalar, integer) - 64-bit to single-precision
                    inst.id = instructionID::SCVTF_64_single_int;
                    break;
                case 3:
                    // 011 - UCVTF (scalar, integer) - 64-bit to single-precision
                    inst.id = instructionID::UCVTF_64_single_int;
                    break;
                case 4:
                    // 100 - FCVTAS (scalar) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTAS_single_64_int;
                    break;
                case 5:
                    // 101 - FCVTAU (scalar) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTAU_single_64_int;
                    break;
                case 6:case 7:
                    inst.id = instructionID::INVALID;
                    break;
                default:
                    UNREACHABLE();
                }
                break;
            case 1:
                if (opcode == 0) {
                    // FCVTPS (scalar, integer) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTPS_single_64_int;
                } else if (opcode == 1) {
                    // FCVTPU (scalar, integer) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTPU_single_64_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 2:
                if (opcode == 0) {
                    // FCVTMS (scalar, integer) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTMS_single_64_int;
                } else if (opcode == 1) {
                    // FCVTMU (scalar, integer) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTMU_single_64_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 3:
                if (opcode == 0) {
                    // FCVTZS (scalar, integer) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTZS_single_64_int;
                } else if (opcode == 1) {
                    // FCVTZU (scalar, integer) - Single-precision to 64-bit
                    inst.id = instructionID::FCVTZU_single_64_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            default:
                UNREACHABLE();
            }
        } else if (type == 1) {
            switch (rmode) {
            case 0:
                switch (opcode) {
                case 0:
                    // 000 - FCVTNS (scalar) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTNS_double_64_int;
                    break;
                case 1:
                    // 001- FCVTNU (scalar) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTNU_double_64_int;
                    break;
                case 2:
                    // 010 - SCVTF (scalar, integer) - 64-bit to Double-precision
                    inst.id = instructionID::SCVTF_64_double_int;
                    break;
                case 3:
                    // 011 - UCVTF (scalar, integer) - 64-bit to Double-precision
                    inst.id = instructionID::UCVTF_64_double_int;
                    break;
                case 4:
                    // 100 - FCVTAS (scalar) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTAS_double_64_int;
                    break;
                case 5:
                    // 101 - FCVTAU (scalar) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTAU_double_64_int;
                    break;
                case 6:
                    // 110 - FMOV (general) - Double-precision to 64-bit
                    inst.id = instructionID::FMOV_double_64;
                    break;
                case 7:
                    // 111 - FMOV (general) - 64-bit to double-precision
                    inst.id = instructionID::FMOV_64_double;
                    break;
                default:
                    UNREACHABLE();
                }
                break;
                break;
            case 1:
                if (opcode == 0) {
                    // FCVTPS (scalar, integer) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTPS_double_64_int;
                } else if (opcode == 1) {
                    // FCVTPU (scalar, integer) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTPU_double_64_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 2:
                if (opcode == 0) {
                    // FCVTMS (scalar, integer) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTMS_double_64_int;
                } else if (opcode == 1) {
                    // FCVTMU (scalar, integer) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTMU_double_64_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            case 3:
                if (opcode == 0) {
                    // FCVTZS (scalar, integer) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTZS_double_64_int;
                } else if (opcode == 1) {
                    // FCVTZU (scalar, integer) - Double-precision to 64-bit
                    inst.id = instructionID::FCVTZU_double_64_int;
                } else {
                    inst.id = instructionID::INVALID;
                }
                break;
            default:
                UNREACHABLE();
            }
        } else if (type == 2) {
            if (rmode == 1) {
                if (opcode == 6) {
                    // FMOV (general) - Top half of 128-bit to 64-bit
                    inst.id = instructionID::FMOV_top128_64;
                } else if (opcode == 7) {
                    // FMOV (general) - 64-bit to top half of 128-bit
                    inst.id = instructionID::FMOV_64_top128;
                } else {
                    inst.id = instructionID::INVALID;
                }
            } else {
                inst.id = instructionID::INVALID;
            }
        } else {
            inst.id = instructionID::INVALID;
        }
    }
}

void decodeFloatDP1Source(uint32_t bits, DecodedInstruction& inst) {
    auto M = bitRange<31, 31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto type = bitRange<22, 23>(bits);
    auto opcode = bitRange<15, 20>(bits);

    if ((M == 1) || (S == 1) || (type == 2) || (opcode > 15)) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (type) {
    case 0:
        switch (opcode) {
        case 0: // 000000 - FMOV (register) - Single-precision variant
            inst.id = instructionID::FMOV_reg_single;
            break;
        case 1: // 000001 - FABS (scalar) - Single-precision variant
            inst.id = instructionID::FABS_single;
            break;
        case 2: // 000010 - FNEG (scalar) - Single-precision variant
            inst.id = instructionID::FNEG_single;
            break;
        case 3: // 000011 - FSQRT (scalar) - Single-precision variant
            inst.id = instructionID::FSQRT_single;
            break;
        case 5: // 000101 - FCVT - Single-precision to double-precision variant
            inst.id = instructionID::FCVT_single_double;
            break;
        case 7: // 000111 - FCVT - Single-precision to half-precision variant
            inst.id = instructionID::FCVT_single_half;
            break;
        case 8: // 001000 - FRINTN (scalar) - Single-precision variant
            inst.id = instructionID::FRINTN_single;
            break;
        case 9: // 001001 - FRINTP (scalar) - Single-precision variant
            inst.id = instructionID::FRINTP_single;
            break;
        case 10: // 001010 - FRINTM (scalar) - Single-precision variant
            inst.id = instructionID::FRINTM_single;
            break;
        case 11: // 001011 - FRINTZ (scalar) - Single-precision variant
            inst.id = instructionID::FRINTZ_single;
            break;
        case 12: // 001100 - FRINTA (scalar) - Single-precision variant
            inst.id = instructionID::FRINTA_single;
            break;
        case 14: // 001110 - FRINTX (scalar) - Single-precision variant
            inst.id = instructionID::FRINTX_single;
            break;
        case 15: // 001111 - FRINTI (scalar) - Single-precision variant
            inst.id = instructionID::FRINTI_single;
            break;
        case 4:case 6:case 13: // 000100 & 000110 & 001101 - Unallocated
            inst.id = instructionID::INVALID;
            break;
        default:
            UNREACHABLE();
        }
        break;
    case 1:
        switch (opcode) {
        case 0: // 000000 - FMOV (register) - Double-precision variant
            inst.id = instructionID::FMOV_reg_double;
            break;
        case 1: // 000001 - FABS (scalar) - Double-precision variant
            inst.id = instructionID::FABS_double;
            break;
        case 2: // 000010 - FNEG (scalar) - Double-precision variant
            inst.id = instructionID::FNEG_double;
            break;
        case 3: // 000011 - FSQRT (scalar) - Double-precision variant
            inst.id = instructionID::FSQRT_double;
            break;
        case 4: // 000100 - FCVT - Double-precision to single-precision variant
            inst.id = instructionID::FCVT_double_single;
            break;
        case 7: // 000111 - FCVT - Double-precision to half-precision variant
            inst.id = instructionID::FCVT_double_half;
            break;
        case 8: // 001000 - FRINTN (scalar) - Double-precision variant
            inst.id = instructionID::FRINTN_double;
            break;
        case 9: // 001001 - FRINTP (scalar) - Double-precision variant
            inst.id = instructionID::FRINTP_double;
            break;
        case 10: // 001010 - FRINTM (scalar) - Double-precision variant
            inst.id = instructionID::FRINTM_double;
            break;
        case 11: // 001011 - FRINTZ (scalar) - Double-precision variant
            inst.id = instructionID::FRINTZ_double;
            break;
        case 12: // 001100 - FRINTA (scalar) - Double-precision variant
            inst.id = instructionID::FRINTA_double;
            break;
        case 14: // 001110 - FRINTX (scalar) - Double-precision variant
            inst.id = instructionID::FRINTX_double;
            break;
        case 15: // 001111 - FRINTI (scalar) - Double-precision variant
            inst.id = instructionID::FRINTI_double;
            break;
        case 5:case 6:case 13: // 000101 & 000110 & 001101 - Unallocated
            inst.id = instructionID::INVALID;
            break;
        default:
            UNREACHABLE();
        }
        break;
    case 2:
        inst.id = instructionID::INVALID;
        break;
    case 3:
        if (opcode == 4) {
            // 000100 - FCVT - Half-precision to single-precision variant
            inst.id = instructionID::FCVT_half_single;
        } else if (opcode == 5) {
            // 000101 - FCVT - Half-precision to double-precision variant
            inst.id = instructionID::FCVT_half_double;
        } else {
            inst.id = instructionID::INVALID;
        }
        break;
    default:
        UNREACHABLE();
    }
}

void decodeFloatCmp(uint32_t bits, DecodedInstruction& inst) {
    auto M = bitRange<31, 31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto type = bitRange<22, 23>(bits);
    auto op = bitRange<14, 15>(bits);
    auto opcode2 = bitRange<0, 4>(bits);

    if ((M == 1) || (S == 1) || (type > 1) || (op != 0) || ((opcode2 & 0x7) != 0)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (type == 0) {
        if (opcode2 == 0) {
            // 00000 - FCMP
            inst.id = instructionID::FCMP_single;
        } else if (opcode2 == 8) {
            // 01000 - FCMP
            inst.id = instructionID::FCMP_single_zero;
        } else if (opcode2 == 16) {
            // 10000 - FCMPE
            inst.id = instructionID::FCMPE_single;
        } else if (opcode2 == 24) {
            // 11000 - FCMPE
            inst.id = instructionID::FCMPE_single_zero;
        }
    } else {
        // type == 1
        if (opcode2 == 0) {
            // 00000 - FCMP
            inst.id = instructionID::FCMP_double;
        } else if (opcode2 == 8) {
            // 01000 - FCMP
            inst.id = instructionID::FCMP_double_zero;
        } else if (opcode2 == 16) {
            // 10000 - FCMPE
            inst.id = instructionID::FCMPE_double;
        } else if (opcode2 == 24) {
            // 11000 - FCMPE
            inst.id = instructionID::FCMPE_double_zero;
        }
    }
}

void decodeFloatImm(uint32_t bits, DecodedInstruction& inst) {
    auto M = bitRange<31, 31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto type = bitRange<22, 23>(bits);
    auto imm5 = bitRange<5, 9>(bits);

    if ((M == 1) || (S == 1) || (type > 1) || (imm5 != 0)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (type == 0) {
        // FMOV (scalar, immediate) - Single-precision variant
        inst.id = instructionID::FMOV_imm_single;
    } else {
        // type == 1 - FMOV (scalar, immediate) - Double-precision variant
        inst.id = instructionID::FMOV_imm_double;
    }
}

void decodeFloatCondCmp(uint32_t bits, DecodedInstruction& inst) {
    auto M = bitRange<31, 31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto type = bitRange<22, 23>(bits);
    auto op = bitRange<4, 4>(bits);

    if ((M == 1) || (S == 1) || (type > 1)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (type == 0) {
        if (op == 0) {
            // FCCMP - Single-precision variant
            inst.id = instructionID::FCCMP_single;
        } else {
            // op == 1 - FCCMPE - Single-precision variant
            inst.id = instructionID::FCCMPE_single;
        }
    } else {
        // type == 1
        if (op == 0) {
            // FCCMP - Double-precision variant
            inst.id = instructionID::FCCMP_double;
        } else {
            // op == 1 - FCCMPE - Double-precision variant
            inst.id = instructionID::FCCMPE_double;
        }
    }
}

void decodeFloatDP2Source(uint32_t bits, DecodedInstruction& inst) {
    auto M = bitRange<31, 31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto type = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 15>(bits);

    if ((M == 1) || (S == 1) || (type > 1) || (opcode > 8)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (type == 0) {
        switch (opcode) {
        case 0: // 0000 - FMUL (scalar) - Single-precision variant
            inst.id = instructionID::FMUL_single;
            break;
        case 1: // 0001 - FDIV (scalar) - Single-precision variant
            inst.id = instructionID::FDIV_single;
            break;
        case 2: // 0010 - FADD (scalar) - Single-precision variant
            inst.id = instructionID::FADD_single;
            break;
        case 3: // 0011 - FSUB (scalar) - Single-precision variant
            inst.id = instructionID::FSUB_single;
            break;
        case 4: // 0100 - FMAX (scalar) - Single-precision variant
            inst.id = instructionID::FMAX_single;
            break;
        case 5: // 0101 - FMIN (scalar) - Single-precision variant
            inst.id = instructionID::FMIN_single;
            break;
        case 6: // 0110 - FMAXNM (scalar) - Single-precision variant
            inst.id = instructionID::FMAXNM_single;
            break;
        case 7: // 0111 - FMINNM (scalar) - Single-precision variant
            inst.id = instructionID::FMINNM_single;
            break;
        case 8: // 1000 - FNMUL (scalar) - Single-precision variant
            inst.id = instructionID::FNMUL_single;
            break;
        default:
            UNREACHABLE();
        }
    } else {
        // type == 1
        switch (opcode) {
        case 0: // 0000 - FMUL (scalar) - Double-precision variant
            inst.id = instructionID::FMUL_double;
            break;
        case 1: // 0001 - FDIV (scalar) - Double-precision variant
            inst.id = instructionID::FDIV_double;
            break;
        case 2: // 0010 - FADD (scalar) - Double-precision variant
            inst.id = instructionID::FADD_double;
            break;
        case 3: // 0011 - FSUB (scalar) - Double-precision variant
            inst.id = instructionID::FSUB_double;
            break;
        case 4: // 0100 - FMAX (scalar) - Double-precision variant
            inst.id = instructionID::FMAX_double;
            break;
        case 5: // 0101 - FMIN (scalar) - Double-precision variant
            inst.id = instructionID::FMIN_double;
            break;
        case 6: // 0110 - FMAXNM (scalar) - Double-precision variant
            inst.id = instructionID::FMAXNM_double;
            break;
        case 7: // 0111 - FMINNM (scalar) - Double-precision variant
            inst.id = instructionID::FMINNM_double;
            break;
        case 8: // 1000 - FNMUL (scalar) - Double-precision variant
            inst.id = instructionID::FNMUL_double;
            break;
        default:
            UNREACHABLE();
        }
    }
}

void decodeFloatCondSel(uint32_t bits, DecodedInstruction& inst) {
    auto M = bitRange<31, 31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto type = bitRange<22, 23>(bits);

    if ((M == 1) || (S == 1) || (type > 1)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (type == 0) {
        // FCSEL - Single-precision variant
        inst.id = instructionID::FCSEL_single;
    } else {
        // type == 1 - FCSEL - Double-precision variant
        inst.id = instructionID::FCSEL_double;
    }
}

void decodeFloatDP3Source(uint32_t bits, DecodedInstruction& inst) {
    auto M = bitRange<31, 31>(bits);
    auto S = bitRange<29, 29>(bits);
    auto type = bitRange<22, 23>(bits);
    auto op1 = bitRange<21, 21>(bits);
    auto op0 = bitRange<15, 15>(bits);

    if ((M == 1) || (S == 1) || (type > 1)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (type == 0) {
        if (op1 == 0) {
            if (op0 == 0) {
                // FMADD - Single-precision variant
                inst.id = instructionID::FMADD_single;
            } else {
                // op0 == 1
                // FMSUB - Single-precision variant
                inst.id = instructionID::FMSUB_single;
            }
        } else {
            // op1 == 1
            if (op0 == 0) {
                // FNMADD - Single-precision variant
                inst.id = instructionID::FNMADD_single;
            } else {
                // op0 == 1
                // FNMSUB - Single-precision variant
                inst.id = instructionID::FNMSUB_single;
            }
        }
    } else {
        // type == 1
        if (op1 == 0) {
            if (op0 == 0) {
                // FMADD - Double-precision variant
                inst.id = instructionID::FMADD_double;
            } else {
                // op0 == 1
                // FMSUB - Double-precision variant
                inst.id = instructionID::FMSUB_double;
            }
        } else {
            // op1 == 1
            if (op0 == 0) {
                // FNMADD - Double-precision variant
                inst.id = instructionID::FNMADD_double;
            } else {
                // op0 == 1
                // FNMSUB - Double-precision variant
                inst.id = instructionID::FNMSUB_double;
            }
        }
    }
}

void decodeFloat(uint32_t bits, DecodedInstruction& inst) {
    auto op1 = bitRange<23, 24>(bits);
    auto op2 = bitRange<19, 22>(bits);
    auto op4 = bitRange<10, 15>(bits);
    if (op1 < 2) {
        // 0x
        if (testBit<2>(op2)) {
            // x1xx
            if (testBit<0>(op4)) {
                if (testBit<1>(op4)) {
                    // xxxx11 - Floating-point conditional select
                    decodeFloatCondSel(bits, inst);
                } else {
                    // xxxx01 - Floating-point conditional compare
                    decodeFloatCondCmp(bits, inst);
                }
            } else if (testBit<1>(op4)) {
                // xxxx10 - Floating-point data-processing (2 source)
                decodeFloatDP2Source(bits, inst);
            } else if (testBit<2>(op4)) {
                // xxx100 - Floating-point immediate
                decodeFloatImm(bits, inst);
            } else if (testBit<3>(op4)) {
                // xx1000 - Floating-point compare
                decodeFloatCmp(bits, inst);
            } else if (testBit<4>(op4)) {
                // x10000 - Floating-point data-processing (1 source)
                decodeFloatDP1Source(bits, inst);
            } else if (testBit<5>(op4)) {
                // 100000 - Unallocated
                inst.id = instructionID::INVALID;
            } else {
                // 000000 - Conversion between floating-point and integer
                decodeConvertFloatAndInt(bits, inst);
            }
        } else {
            // x0xx - Conversion between floating-point and fixed-point
            decodeConvertFloatAndFixed(bits, inst);
        }
    } else {
        // 1x - Floating-point data-processing (3 source)
        decodeFloatDP3Source(bits, inst);
    }
}

} // namespace cpu::decoder