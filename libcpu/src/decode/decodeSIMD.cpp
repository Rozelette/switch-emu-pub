#include "decode.h"
#include "bitutils.h"
#include "unreachable.h"
#include "log.h"

namespace cpu::decoder {

void decodeCryptographicAES(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 16>(bits);

    if (size != 0) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (opcode) {
    case 4:
        inst.id = instructionID::AESE;
        return;
    case 5:
        inst.id = instructionID::AESD;
        return;
    case 6:
        inst.id = instructionID::AESMC;
        return;
    case 7:
        inst.id = instructionID::AESIMC;
        return;
    default:
        inst.id = instructionID::INVALID;
        return;
    }
}

void decodeCryptographic3Reg(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 14>(bits);

    if (size != 0) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (opcode) {
    case 0:
        inst.id = instructionID::SHA1C;
        return;
    case 1:
        inst.id = instructionID::SHA1P;
        return;
    case 2:
        inst.id = instructionID::SHA1M;
        return;
    case 3:
        inst.id = instructionID::SHA1SU0;
        return;
    case 4:
        inst.id = instructionID::SHA256H;
        return;
    case 5:
        inst.id = instructionID::SHA256H2;
        return;
    case 6:
        inst.id = instructionID::SHA256SU1;
        return;
    default:
        inst.id = instructionID::INVALID;
        return;
    }
}

void decodeCryptographic2Reg(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 16>(bits);

    if (size != 0) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (opcode) {
    case 0:
        inst.id = instructionID::SHA1H;
        return;
    case 1:
        inst.id = instructionID::SHA1SU1;
        return;
    case 2:
        inst.id = instructionID::SHA256SU0;
        return;
    default:
        inst.id = instructionID::INVALID;
        return;
    }
}

void decodeSIMDScalarCopy(uint32_t bits, DecodedInstruction& inst) {
    if (!testBit<29>(bits) && (bitRange<16, 19>(bits) != 0) && (bitRange<11, 15>(bits) == 0)) {
        inst.id = instructionID::DUP_element_scalar;
    } else {
        inst.id = instructionID::INVALID;
    }
}

void decodeSIMDScalar2Reg(uint32_t bits, DecodedInstruction& inst) {
    bool U = testBit<29>(bits);
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 16>(bits);

    if (!U) {
        switch (opcode) {
        case 3:
            inst.id = instructionID::SUQADD_scalar;
            return;
        case 7:
            inst.id = instructionID::SQABS_scalar;
            return;
        case 8:
            inst.id = instructionID::CMGT_scalar_zero;
            return;
        case 9:
            inst.id = instructionID::CMEQ_scalar_zero;
            return;
        case 10:
            inst.id = instructionID::CMLT_scalar_zero;
            return;
        case 11:
            inst.id = instructionID::ABS_scalar;
            return;
        case 12:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMGT_scalar_zero;
            }
            return;
        case 13:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMEQ_scalar_zero;
            }
            return;
        case 14:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMLT_scalar_zero;
            }
            return;
        case 20:
            inst.id = instructionID::SQXTN_scalar;
            return;
        case 26:
            if (size < 2) {
                inst.id = instructionID::FCVTNS_scalar;
            } else {
                inst.id = instructionID::FCVTPS_scalar;
            }
            return;
        case 27:
            if (size < 2) {
                inst.id = instructionID::FCVTMS_scalar;
            } else {
                inst.id = instructionID::FCVTZS_scalar_int;
            }
            return;
        case 28:
            if (size < 2) {
                inst.id = instructionID::FCVTAS_scalar;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 29:
            if (size < 2) {
                inst.id = instructionID::SCVTF_scalar_int;
            } else {
                inst.id = instructionID::FRECPE_scalar;
            }
            return;
        case 31:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FRECPX_scalar;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        switch (opcode) {
        case 3:
            inst.id = instructionID::USQADD_scalar;
            return;
        case 7:
            inst.id = instructionID::SQNEG_scalar;
            return;
        case 8:
            inst.id = instructionID::CMGE_scalar_zero;
            return;
        case 9:
            inst.id = instructionID::CMLE_scalar_zero;
            return;
        case 11:
            inst.id = instructionID::NEG_scalar;
            return;
        case 12:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMGE_scalar_zero;
            }
            return;
        case 13:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMLE_scalar_zero;
            }
            return;
        case 18:
            inst.id = instructionID::SQXTUN_scalar;
            return;
        case 20:
            inst.id = instructionID::UQXTN_scalar;
            return;
        case 22:
            if (size < 2) {
                inst.id = instructionID::FCVTXN_scalar;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 26:
            if (size < 2) {
                inst.id = instructionID::FCVTNU_scalar;
            } else {
                inst.id = instructionID::FCVTPU_scalar;
            }
            return;
        case 27:
            if (size < 2) {
                inst.id = instructionID::FCVTMU_scalar;
            } else {
                inst.id = instructionID::FCVTZU_scalar_int;
            }
            return;
        case 28:
            if (size < 2) {
                inst.id = instructionID::FCVTAU_scalar;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 29:
            if (size < 2) {
                inst.id = instructionID::UCVTF_scalar_int;
            } else {
                inst.id = instructionID::FRSQRTE_scalar;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeSIMDScalarPairwise(uint32_t bits, DecodedInstruction& inst) {
    bool U = testBit<29>(bits);
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 16>(bits);

    if (!U) {
        if (opcode == 0b11011) {
            inst.id = instructionID::ADDP_scalar;
        } else {
            inst.id = instructionID::INVALID;
        }
    } else {
        if (size < 2) {
            if (opcode == 0b01100) {
                inst.id = instructionID::FMAXNMP_scalar;
            } else if (opcode == 0b01101) {
                inst.id = instructionID::FADDP_scalar;
            } else if (opcode == 0b01111) {
                inst.id = instructionID::FMAXP_scalar;
            } else {
                inst.id = instructionID::INVALID;
            }
        } else {
            if (opcode == 0b01100) {
                inst.id = instructionID::FMINNMP_scalar;
            } else if (opcode == 0b01111) {
                inst.id = instructionID::FMINP_scalar;
            } else {
                inst.id = instructionID::INVALID;
            }
        }
    }
}

void decodeSIMDScalar3Different(uint32_t bits, DecodedInstruction& inst) {
    bool U = testBit<29>(bits);
    auto opcode = bitRange<12, 15>(bits);

    if (!U) {
        if (opcode == 0b1001) {
            inst.id = instructionID::SQDMLAL_scalar;
        } else if (opcode == 0b1011) {
            inst.id = instructionID::SQDMLSL_scalar;
        } else if (opcode == 0b1101) {
            inst.id = instructionID::SQDMULL_scalar;
        } else {
            inst.id = instructionID::INVALID;
        }
    } else {
        inst.id = instructionID::INVALID;
    }
}

void decodeSIMDScalar3Same(uint32_t bits, DecodedInstruction& inst) {
    bool U = testBit<29>(bits);
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<11, 15>(bits);

    if (!U) {
        switch (opcode) {
        case 1:
            inst.id = instructionID::SQADD_scalar;
            return;
        case 5:
            inst.id = instructionID::SQSUB_scalar;
            return;
        case 6:
            inst.id = instructionID::CMGT_scalar_reg;
            return;
        case 7:
            inst.id = instructionID::CMGE_scalar_reg;
            return;
        case 8:
            inst.id = instructionID::SSHL_scalar;
            return;
        case 9:
            inst.id = instructionID::SQSHL_scalar_reg;
            return;
        case 10:
            inst.id = instructionID::SRSHL_scalar;
            return;
        case 11:
            inst.id = instructionID::SQRSHL_scalar;
            return;
        case 16:
            inst.id = instructionID::ADD_scalar;
            return;
        case 17:
            inst.id = instructionID::CMTST_scalar;
            return;
        case 22:
            inst.id = instructionID::SQDMULH_scalar;
            return;
        case 27:
            if (size < 2) {
                inst.id = instructionID::FMULX_scalar;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 28:
            if (size < 2) {
                inst.id = instructionID::FCMEQ_scalar_reg;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 31:
            if (size < 2) {
                inst.id = instructionID::FRECPS_scalar;
            } else {
                inst.id = instructionID::FRSQRTS_scalar;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        switch (opcode) {
        case 1:
            inst.id = instructionID::UQADD_scalar;
            return;
        case 5:
            inst.id = instructionID::UQSUB_scalar;
            return;
        case 6:
            inst.id = instructionID::CMHI_scalar_reg;
            return;
        case 7:
            inst.id = instructionID::CMHS_scalar_reg;
            return;
        case 8:
            inst.id = instructionID::USHL_scalar;
            return;
        case 9:
            inst.id = instructionID::UQSHL_scalar_reg;
            return;
        case 10:
            inst.id = instructionID::URSHL_scalar;
            return;
        case 11:
            inst.id = instructionID::UQRSHL_scalar;
            return;
        case 16:
            inst.id = instructionID::SUB_scalar;
            return;
        case 17:
            inst.id = instructionID::CMEQ_scalar_reg;
            return;
        case 22:
            inst.id = instructionID::SQRDMULH_scalar;
            return;
        case 26:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FABD_scalar;
            }
            return;
        case 28:
            if (size < 2) {
                inst.id = instructionID::FCMGE_scalar_reg;
            } else {
                inst.id = instructionID::FCMGT_scalar_reg;
            }
            return;
        case 29:
            if (size < 2) {
                inst.id = instructionID::FACGE_scalar;
            } else {
                inst.id = instructionID::FACGT_scalar;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeSIMDScalarShiftImmediate(uint32_t bits, DecodedInstruction& inst) {
    if (bitRange<19, 22>(bits) == 0) {
        inst.id = instructionID::INVALID;
        return;
    }

    bool U = testBit<29>(bits);
    auto opcode = bitRange<11, 15>(bits);

    if (!U) {
        switch (opcode) {
        case 0:
            inst.id = instructionID::SSHR_scalar;
            return;
        case 2:
            inst.id = instructionID::SSRA_scalar;
            return;
        case 4:
            inst.id = instructionID::SRSHR_scalar;
            return;
        case 6:
            inst.id = instructionID::SRSRA_scalar;
            return;
        case 10:
            inst.id = instructionID::SHL_scalar;
            return;
        case 14:
            inst.id = instructionID::SQSHL_scalar_imm;
            return;
        case 18:
            inst.id = instructionID::SQSHRN_scalar;
            return;
        case 19:
            inst.id = instructionID::SQRSHRN_scalar;
            return;
        case 28:
            inst.id = instructionID::SCVTF_scalar_fixed;
            return;
        case 31:
            inst.id = instructionID::FCVTZS_scalar_fixed;
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        switch (opcode) {
        case 0:
            inst.id = instructionID::USHR_scalar;
            return;
        case 2:
            inst.id = instructionID::USRA_scalar;
            return;
        case 4:
            inst.id = instructionID::URSHR_scalar;
            return;
        case 6:
            inst.id = instructionID::URSRA_scalar;
            return;
        case 8:
            inst.id = instructionID::SRI_scalar;
            return;
        case 10:
            inst.id = instructionID::SLI_scalar;
            return;
        case 12:
            inst.id = instructionID::SQSHLU_scalar;
            return;
        case 14:
            inst.id = instructionID::UQSHL_scalar_imm;
            return;
        case 16:
            inst.id = instructionID::SQSHRUN_scalar;
            return;
        case 17:
            inst.id = instructionID::SQRSHRUN_scalar;
            return;
        case 18:
            inst.id = instructionID::UQSHRN_scalar;
            return;
        case 19:
            inst.id = instructionID::UQRSHRN_scalar;
            return;
        case 28:
            inst.id = instructionID::UCVTF_scalar_fixed;
            return;
        case 31:
            inst.id = instructionID::FCVTZU_scalar_fixed;
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeSIMDScalarXIndexed(uint32_t bits, DecodedInstruction& inst) {
    bool U = testBit<29>(bits);
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 15>(bits);

    if (!U) {
        switch (opcode) {
        case 1:
            if (size > 1) {
                inst.id = instructionID::FMLA_scalar_element;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 3:
            inst.id = instructionID::SQDMLAL_scalar_element;
            return;
        case 5:
            if (size > 1) {
                inst.id = instructionID::FMLS_scalar_element;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 7:
            inst.id = instructionID::SQDMLSL_scalar_element;
            return;
        case 9:
            if (size > 1) {
                inst.id = instructionID::FMUL_scalar_element;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 11:
            inst.id = instructionID::SQDMULL_scalar_element;
            return;
        case 12:
            inst.id = instructionID::SQDMULH_scalar_element;
            return;
        case 13:
            inst.id = instructionID::SQRDMULH_scalar_element;
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        if ((size > 1) && (opcode == 9)) {
            inst.id = instructionID::FMULX_scalar_element;
        } else {
            inst.id = instructionID::INVALID;
        }
    }
}

void decodeSIMDTableLookup(uint32_t bits, DecodedInstruction& inst) {
    if (bitRange<22, 23>(bits) != 0) {
        inst.id = instructionID::INVALID;
        return;
    }

    auto len = bitRange<13, 14>(bits);
    auto op = bitRange<12, 12>(bits);

    switch (len) {
    case 0:
        if (op == 0) {
            inst.id = instructionID::TBL_one_reg;
        } else {
            inst.id = instructionID::TBX_one_reg;
        }
        return;
    case 1:
        if (op == 0) {
            inst.id = instructionID::TBL_two_reg;
        } else {
            inst.id = instructionID::TBX_two_reg;
        }
        return;
    case 2:
        if (op == 0) {
            inst.id = instructionID::TBL_three_reg;
        } else {
            inst.id = instructionID::TBX_three_reg;
        }
        return;
    case 3:
        if (op == 0) {
            inst.id = instructionID::TBL_four_reg;
        } else {
            inst.id = instructionID::TBX_four_reg;
        }
        return;
    default:
        inst.id = instructionID::INVALID;
        return;
    }
}

void decodeSIMDPermute(uint32_t bits, DecodedInstruction& inst) {
    auto opcode = bitRange<12, 14>(bits);

    switch (opcode) {
    case 1:
        inst.id = instructionID::UZP1;
        return;
    case 2:
        inst.id = instructionID::TRN1;
        return;
    case 3:
        inst.id = instructionID::ZIP1;
        return;
    case 5:
        inst.id = instructionID::UZP2;
        return;
    case 6:
        inst.id = instructionID::TRN2;
        return;
    case 7:
        inst.id = instructionID::ZIP2;
        return;
    default:
        inst.id = instructionID::INVALID;
        return;
    }
}

void decodeSIMDExtract(uint32_t bits, DecodedInstruction& inst) {
    if (bitRange<22, 23>(bits) == 0) {
        inst.id = instructionID::EXT;
    } else {
        inst.id = instructionID::INVALID;
    }
}

void decodeSIMDCopy(uint32_t bits, DecodedInstruction& inst) {
    bool Q = testBit<30>(bits);
    bool op = testBit<29>(bits);
    auto imm5 = bitRange<16, 20>(bits);
    auto imm4 = bitRange<11, 14>(bits);

    if ((imm5 & 0b1111) == 0) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (!op) {
        switch (imm4) {
        case 0:
            inst.id = instructionID::DUP_element_vec;
            return;
        case 1:
            inst.id = instructionID::DUP_general;
            return;
        case 3:
            if (Q) {
                inst.id = instructionID::INS_general;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 5:
            if (Q) {
                inst.id = instructionID::SMOV_64;
            } else {
                inst.id = instructionID::SMOV_32;
            }
            return;
        case 7:
            if (Q) {
                if ((imm5 & 0b1111) == 8) {
                    inst.id = instructionID::UMOV_64;
                } else {
                    inst.id = instructionID::INVALID;
                }
            } else {
                inst.id = instructionID::UMOV_32;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        if (Q) {
            inst.id = instructionID::INS_element;
        } else {
            inst.id = instructionID::INVALID;
        }
    }
}

void decodeSIMD2Reg(uint32_t bits, DecodedInstruction& inst) {
    bool Q = testBit<30>(bits);
    bool U = testBit<29>(bits);
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 16>(bits);

    if (!U) {
        switch (opcode) {
        case 0:
            inst.id = instructionID::REV64_vec;
            return;
        case 1:
            inst.id = instructionID::REV16_vec;
            return;
        case 2:
            inst.id = instructionID::SADDLP_vec;
            return;
        case 3:
            inst.id = instructionID::SUQADD_vec;
            return;
        case 4:
            inst.id = instructionID::CLS_vec;
            return;
        case 5:
            inst.id = instructionID::CNT_vec;
            return;
        case 6:
            inst.id = instructionID::SADALP_vec;
            return;
        case 7:
            inst.id = instructionID::SQABS_vec;
            return;
        case 8:
            inst.id = instructionID::CMGT_vec_zero;
            return;
        case 9:
            inst.id = instructionID::CMEQ_vec_zero;
            return;
        case 10:
            inst.id = instructionID::CMLT_vec_zero;
            return;
        case 11:
            inst.id = instructionID::ABS_vec;
            return;
        case 12:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMGT_vec_zero;
            }
            return;
        case 13:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMEQ_vec_zero;
            }
            return;
        case 14:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMLT_vec_zero;
            }
            return;
        case 15:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FABS_vec;
            }
            return;
        case 18:
            if (!Q) {
                inst.id = instructionID::XTN_vec;
            } else {
                inst.id = instructionID::XTN2_vec;
            }
            return;
        case 20:
            if (!Q) {
                inst.id = instructionID::SQXTN_vec;
            } else {
                inst.id = instructionID::SQXTN2_vec;
            }
            return;
        case 22:
            if (size < 2) {
                if (!Q) {
                    inst.id = instructionID::FCVTN_vec;
                } else {
                    inst.id = instructionID::FCVTN2_vec;
                }
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 23:
            if (size < 2) {
                if (!Q) {
                    inst.id = instructionID::FCVTL_vec;
                } else {
                    inst.id = instructionID::FCVTL2_vec;
                }
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 24:
            if (size < 2) {
                inst.id = instructionID::FRINTN_vec;
            } else {
                inst.id = instructionID::FRINTP_vec;
            }
            return;
        case 25:
            if (size < 2) {
                inst.id = instructionID::FRINTM_vec;
            } else {
                inst.id = instructionID::FRINTZ_vec;
            }
            return;
        case 26:
            if (size < 2) {
                inst.id = instructionID::FCVTNS_vec;
            } else {
                inst.id = instructionID::FCVTPS_vec;
            }
            return;
        case 27:
            if (size < 2) {
                inst.id = instructionID::FCVTMS_vec;
            } else {
                inst.id = instructionID::FCVTZS_vec_int;
            }
            return;
        case 28:
            if (size < 2) {
                inst.id = instructionID::FCVTAS_vec;
            } else {
                inst.id = instructionID::URECPE_vec;
            }
            return;
        case 29:
            if (size < 2) {
                inst.id = instructionID::SCVTF_vec_int;
            } else {
                inst.id = instructionID::FRECPE_vec;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        switch (opcode) {
        case 0:
            inst.id = instructionID::REV32_vec;
            return;
        case 2:
            inst.id = instructionID::UADDLP_vec;
            return;
        case 3:
            inst.id = instructionID::USQADD_vec;
            return;
        case 4:
            inst.id = instructionID::CLZ_vec;
            return;
        case 5:
            if (size == 0) {
                inst.id = instructionID::NOT_vec;
            } else if (size == 1) {
                inst.id = instructionID::RBIT_vec;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 6:
            inst.id = instructionID::UADALP_vec;
            return;
        case 7:
            inst.id = instructionID::SQNEG_vec;
            return;
        case 8:
            inst.id = instructionID::CMGE_vec_zero;
            return;
        case 9:
            inst.id = instructionID::CMLE_vec_zero;
            return;
        case 11:
            inst.id = instructionID::NEG_vec;
            return;
        case 12:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMGE_vec_zero;
            }
            return;
        case 13:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FCMLE_vec_zero;
            }
            return;
        case 15:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FNEG_vec;
            }
            return;
        case 18:
            if (!Q) {
                inst.id = instructionID::SQXTUN_vec;
            } else {
                inst.id = instructionID::SQXTUN2_vec;
            }
            return;
        case 19:
            if (!Q) {
                inst.id = instructionID::SHLL_vec;
            } else {
                inst.id = instructionID::SHLL2_vec;
            }
            return;
        case 20:
            if (!Q) {
                inst.id = instructionID::UQXTN_vec;
            } else {
                inst.id = instructionID::UQXTN2_vec;
            }
            return;
        case 22:
            if (size < 2) {
                if (!Q) {
                    inst.id = instructionID::FCVTXN_vec;
                } else {
                    inst.id = instructionID::FCVTXN2_vec;
                }
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 24:
            if (size < 2) {
                inst.id = instructionID::FRINTA_vec;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 25:
            if (size < 2) {
                inst.id = instructionID::FRINTX_vec;
            } else {
                inst.id = instructionID::FRINTI_vec;
            }
            return;
        case 26:
            if (size < 2) {
                inst.id = instructionID::FCVTNU_vec;
            } else {
                inst.id = instructionID::FCVTPU_vec;
            }
            return;
        case 27:
            if (size < 2) {
                inst.id = instructionID::FCVTMU_vec;
            } else {
                inst.id = instructionID::FCVTZU_vec_int;
            }
            return;
        case 28:
            if (size < 2) {
                inst.id = instructionID::FCVTAU_vec;
            } else {
                inst.id = instructionID::URSQRTE_vec;
            }
            return;
        case 29:
            if (size < 2) {
                inst.id = instructionID::UCVTF_vec_int;
            } else {
                inst.id = instructionID::FRSQRTE_vec;
            }
            return;
        case 31:
            if (size < 2) {
                inst.id = instructionID::INVALID;
            } else {
                inst.id = instructionID::FSQRT_vec;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeSIMDAcrossLanes(uint32_t bits, DecodedInstruction& inst) {
    bool U = testBit<29>(bits);
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 16>(bits);

    if (!U) {
        if (opcode == 3) {
            inst.id = instructionID::SADDLV_vec;
        } else if (opcode == 10) {
            inst.id = instructionID::SMAXV_vec;
        } else if (opcode == 26) {
            inst.id = instructionID::SMINV_vec;
        } else if (opcode == 27) {
            inst.id = instructionID::ADDV_vec;
        } else {
            inst.id = instructionID::INVALID;
        }
    } else {
        if (opcode == 3) {
            inst.id = instructionID::UADDLV_vec;
        } else if (opcode == 10) {
            inst.id = instructionID::UMAXV_vec;
        } else if (opcode == 12) {
            if (size < 2) {
                inst.id = instructionID::FMAXNMV_vec;
            } else {
                inst.id = instructionID::FMINNMV_vec;
            }
        } else if (opcode == 15) {
            if (size < 2) {
                inst.id = instructionID::FMAXV_vec;
            } else {
                inst.id = instructionID::FMINV_vec;
            }
        } else if (opcode == 26) {
            inst.id = instructionID::UMINV_vec;
        } else {
            inst.id = instructionID::INVALID;
        }
    }
}

void decodeSIMD3Different(uint32_t bits, DecodedInstruction& inst) {
    bool Q = testBit<30>(bits);
    bool U = testBit<29>(bits);
    auto opcode = bitRange<12, 15>(bits);

    if (!U) {
        switch (opcode) {
        case 0:
            if (!Q) {
                inst.id = instructionID::SADDL;
            } else {
                inst.id = instructionID::SADDL2;
            }
            return;
        case 1:
            if (!Q) {
                inst.id = instructionID::SADDW;
            } else {
                inst.id = instructionID::SADDW2;
            }
            return;
        case 2:
            if (!Q) {
                inst.id = instructionID::SSUBL;
            } else {
                inst.id = instructionID::SSUBL2;
            }
            return;
        case 3:
            if (!Q) {
                inst.id = instructionID::SSUBW;
            } else {
                inst.id = instructionID::SSUBW2;
            }
            return;
        case 4:
            if (!Q) {
                inst.id = instructionID::ADDHN;
            } else {
                inst.id = instructionID::ADDHN2;
            }
            return;
        case 5:
            if (!Q) {
                inst.id = instructionID::SABAL;
            } else {
                inst.id = instructionID::SABAL2;
            }
            return;
        case 6:
            if (!Q) {
                inst.id = instructionID::SUBHN;
            } else {
                inst.id = instructionID::SUBHN2;
            }
            return;
        case 7:
            if (!Q) {
                inst.id = instructionID::SABDL;
            } else {
                inst.id = instructionID::SABDL2;
            }
            return;
        case 8:
            if (!Q) {
                inst.id = instructionID::SMLAL_vec;
            } else {
                inst.id = instructionID::SMLAL2_vec;
            }
            return;
        case 9:
            if (!Q) {
                inst.id = instructionID::SQDMLAL_vec;
            } else {
                inst.id = instructionID::SQDMLAL2_vec;
            }
            return;
        case 10:
            if (!Q) {
                inst.id = instructionID::SMLSL_vec;
            } else {
                inst.id = instructionID::SMLSL2_vec;
            }
            return;
        case 11:
            if (!Q) {
                inst.id = instructionID::SQDMLSL_vec;
            } else {
                inst.id = instructionID::SQDMLSL2_vec;
            }
            return;
        case 12:
            if (!Q) {
                inst.id = instructionID::SMULL_vec;
            } else {
                inst.id = instructionID::SMULL2_vec;
            }
            return;
        case 13:
            if (!Q) {
                inst.id = instructionID::SQDMULL_vec;
            } else {
                inst.id = instructionID::SQDMULL2_vec;
            }
            return;
        case 14:
            if (!Q) {
                inst.id = instructionID::PMULL;
            } else {
                inst.id = instructionID::PMULL2;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
    switch (opcode) {
    case 0:
        if (!Q) {
            inst.id = instructionID::UADDL;
        } else {
            inst.id = instructionID::UADDL2;
        }
        return;
    case 1:
        if (!Q) {
            inst.id = instructionID::UADDW;
        } else {
            inst.id = instructionID::UADDW2;
        }
        return;
    case 2:
        if (!Q) {
            inst.id = instructionID::USUBL;
        } else {
            inst.id = instructionID::USUBL2;
        }
        return;
    case 3:
        if (!Q) {
            inst.id = instructionID::USUBW;
        } else {
            inst.id = instructionID::USUBW2;
        }
        return;
    case 4:
        if (!Q) {
            inst.id = instructionID::RADDHN;
        } else {
            inst.id = instructionID::RADDHN2;
        }
        return;
    case 5:
        if (!Q) {
            inst.id = instructionID::UABAL;
        } else {
            inst.id = instructionID::UABAL2;
        }
        return;
    case 6:
        if (!Q) {
            inst.id = instructionID::RSUBHN;
        } else {
            inst.id = instructionID::RSUBHN2;
        }
        return;
    case 7:
        if (!Q) {
            inst.id = instructionID::UABDL;
        } else {
            inst.id = instructionID::UABDL2;
        }
        return;
    case 8:
        if (!Q) {
            inst.id = instructionID::UMLAL_vec;
        } else {
            inst.id = instructionID::UMLAL2_vec;
        }
        return;
    case 10:
        if (!Q) {
            inst.id = instructionID::UMLSL_vec;
        } else {
            inst.id = instructionID::UMLSL2_vec;
        }
        return;
    case 12:
        if (!Q) {
            inst.id = instructionID::UMULL_vec;
        } else {
            inst.id = instructionID::UMULL2_vec;
        }
        return;
    default:
        inst.id = instructionID::INVALID;
        return;
    }
    }
}

void decodeSIMD3Same(uint32_t bits, DecodedInstruction& inst) {
    bool U = testBit<29>(bits);
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<11, 15>(bits);

    if (!U) {
        switch (opcode) {
        case 0:
            inst.id = instructionID::SHADD_vec;
            return;
        case 1:
            inst.id = instructionID::SQADD_vec;
            return;
        case 2:
            inst.id = instructionID::SRHADD_vec;
            return;
        case 3:
            if (size == 0) {
                inst.id = instructionID::AND_vec;
            } else if (size == 1) {
                inst.id = instructionID::BIC_vec_reg;
            } else if (size == 2) {
                inst.id = instructionID::ORR_vec_reg;
            } else if (size == 3) {
                inst.id = instructionID::ORN_vec;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 4:
            inst.id = instructionID::SHSUB_vec;
            return;
        case 5:
            inst.id = instructionID::SQSUB_vec;
            return;
        case 6:
            inst.id = instructionID::CMGT_vec_reg;
            return;
        case 7:
            inst.id = instructionID::CMGE_vec_reg;
            return;
        case 8:
            inst.id = instructionID::SSHL_vec;
            return;
        case 9:
            inst.id = instructionID::SQSHL_vec_reg;
            return;
        case 10:
            inst.id = instructionID::SRSHL_vec;
            return;
        case 11:
            inst.id = instructionID::SQRSHL_vec;
            return;
        case 12:
            inst.id = instructionID::SMAX_vec;
            return;
        case 13:
            inst.id = instructionID::SMIN_vec;
            return;
        case 14:
            inst.id = instructionID::SABD_vec;
            return;
        case 15:
            inst.id = instructionID::SABA_vec;
            return;
        case 16:
            inst.id = instructionID::ADD_vec;
            return;
        case 17:
            inst.id = instructionID::CMTST_vec;
            return;
        case 18:
            inst.id = instructionID::MLA_vec;
            return;
        case 19:
            inst.id = instructionID::MUL_vec;
            return;
        case 20:
            inst.id = instructionID::SMAXP_vec;
            return;
        case 21:
            inst.id = instructionID::SMINP_vec;
            return;
        case 22:
            inst.id = instructionID::SQDMULH_vec;
            return;
        case 23:
            inst.id = instructionID::ADDP_vec;
            return;
        case 24:
            if (size < 2) {
                inst.id = instructionID::FMAXNM_vec;
            } else {
                inst.id = instructionID::FMINNM_vec;
            }
            return;
        case 25:
            if (size < 2) {
                inst.id = instructionID::FMLA_vec;
            } else {
                inst.id = instructionID::FMLS_vec;
            }
            return;
        case 26:
            if (size < 2) {
                inst.id = instructionID::FADD_vec;
            } else {
                inst.id = instructionID::FSUB_vec;
            }
            return;
        case 27:
            if (size < 2) {
                inst.id = instructionID::FMULX_vec;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 28:
            if (size < 2) {
                inst.id = instructionID::FCMEQ_vec_reg;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 29:
            inst.id = instructionID::INVALID;
            return;
        case 30:
            if (size < 2) {
                inst.id = instructionID::FMAX_vec;
            } else {
                inst.id = instructionID::FMIN_vec;
            }
            return;
        case 31:
            if (size < 2) {
                inst.id = instructionID::FRECPS_vec;
            } else {
                inst.id = instructionID::FRSQRTS_vec;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        switch (opcode) {
        case 0:
            inst.id = instructionID::UHADD_vec;
            return;
        case 1:
            inst.id = instructionID::UQADD_vec;
            return;
        case 2:
            inst.id = instructionID::URHADD_vec;
            return;
        case 3:
            if (size == 0) {
                inst.id = instructionID::EOR_vec;
            } else if (size == 1) {
                inst.id = instructionID::BSL_vec;
            } else if (size == 2) {
                inst.id = instructionID::BIT_vec;
            } else if (size == 3) {
                inst.id = instructionID::BIF_vec;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 4:
            inst.id = instructionID::UHSUB_vec;
            return;
        case 5:
            inst.id = instructionID::UQSUB_vec;
            return;
        case 6:
            inst.id = instructionID::CMHI_vec_reg;
            return;
        case 7:
            inst.id = instructionID::CMHS_vec_reg;
            return;
        case 8:
            inst.id = instructionID::USHL_vec;
            return;
        case 9:
            inst.id = instructionID::UQSHL_vec_reg;
            return;
        case 10:
            inst.id = instructionID::URSHL_vec;
            return;
        case 11:
            inst.id = instructionID::UQRSHL_vec;
            return;
        case 12:
            inst.id = instructionID::UMAX_vec;
            return;
        case 13:
            inst.id = instructionID::UMIN_vec;
            return;
        case 14:
            inst.id = instructionID::UABD_vec;
            return;
        case 15:
            inst.id = instructionID::UABA_vec;
            return;
        case 16:
            inst.id = instructionID::SUB_vec;
            return;
        case 17:
            inst.id = instructionID::CMEQ_vec_reg;
            return;
        case 18:
            inst.id = instructionID::MLS_vec;
            return;
        case 19:
            inst.id = instructionID::PMUL_vec;
            return;
        case 20:
            inst.id = instructionID::UMAXP_vec;
            return;
        case 21:
            inst.id = instructionID::UMINP_vec;
            return;
        case 22:
            inst.id = instructionID::SQRDMULH_vec;
            return;
        case 23:
            inst.id = instructionID::INVALID;
            return;
        case 24:
            if (size < 2) {
                inst.id = instructionID::FMAXNMP_vec;
            } else {
                inst.id = instructionID::FMINNMP_vec;
            }
            return;
        case 25:
            inst.id = instructionID::INVALID;
            return;
        case 26:
            if (size < 2) {
                inst.id = instructionID::FADDP_vec;
            } else {
                inst.id = instructionID::FABD_vec;
            }
            return;
        case 27:
            if (size < 2) {
                inst.id = instructionID::FMUL_vec;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 28:
            if (size < 2) {
                inst.id = instructionID::FCMGE_vec_reg;
            } else {
                inst.id = instructionID::FCMGT_vec_reg;
            }
            return;
        case 29:
            if (size < 2) {
                inst.id = instructionID::FACGE_vec;
            } else {
                inst.id = instructionID::FACGT_vec;
            }
            return;
        case 30:
            if (size < 2) {
                inst.id = instructionID::FMAXP_vec;
            } else {
                inst.id = instructionID::FMINP_vec;
            }
            return;
        case 31:
            if (size < 2) {
                inst.id = instructionID::FDIV_vec;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeSIMDModifiedImmediate(uint32_t bits, DecodedInstruction& inst) {
    bool Q = testBit<30>(bits);
    bool op = testBit<29>(bits);
    auto cmode = bitRange<12, 15>(bits);

    if (testBit<11>(bits)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (!op) {
        switch (cmode) {
        case 0:case 2:case 4:case 6:
            inst.id = instructionID::MOVI_32_shifted_imm;
            return;
        case 1:case 3:case 5:case 7:
            inst.id = instructionID::ORR_vec_imm_32;
            return;
        case 8:case 10:
            inst.id = instructionID::MOVI_16_shifted_imm;
            return;
        case 9:case 11:
            inst.id = instructionID::ORR_vec_imm_16;
            return;
        case 12:case 13:
            inst.id = instructionID::MOVI_32_shifting_ones;
            return;
        case 14:
            inst.id = instructionID::MOVI_8_imm;
            return;
        case 15:
            inst.id = instructionID::FMOV_vec_imm_single;
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        switch (cmode) {
        case 0:case 2:case 4:case 6:
            inst.id = instructionID::MVNI_32_shifted_imm;
            return;
        case 1:case 3:case 5:case 7:
            inst.id = instructionID::BIC_vec_imm_32;
            return;
        case 8:case 10:
            inst.id = instructionID::MVNI_16_shifted_imm;
            return;
        case 9:case 11:
            inst.id = instructionID::BIC_vec_imm_16;
            return;
        case 12:case 13:
            inst.id = instructionID::MVNI_32_shifting_ones;
            return;
        case 14:
            if (Q) {
                inst.id = instructionID::MOVI_64_vec;
            } else {
                inst.id = instructionID::MOVI_64_scalar;
            }
            return;
        case 15:
            if (Q) {
                inst.id = instructionID::FMOV_vec_imm_double;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeSIMDShiftImmediate(uint32_t bits, DecodedInstruction& inst) {
    bool Q = testBit<30>(bits);
    bool U = testBit<29>(bits);
    auto opcode = bitRange<11, 15>(bits);

    if (!U) {
        switch (opcode) {
        case 0:
            inst.id = instructionID::SSHR_vec;
            return;
        case 2:
            inst.id = instructionID::SSRA_vec;
            return;
        case 4:
            inst.id = instructionID::SRSHR_vec;
            return;
        case 6:
            inst.id = instructionID::SRSRA_vec;
            return;
        case 10:
            inst.id = instructionID::SHL_vec;
            return;
        case 14:
            inst.id = instructionID::SQSHL_vec_imm;
            return;
        case 16:
            if (!Q) {
                inst.id = instructionID::SHRN_vec;
            } else {
                inst.id = instructionID::SHRN2_vec;
            }
            return;
        case 17:
            if (!Q) {
                inst.id = instructionID::RSHRN_vec;
            } else {
                inst.id = instructionID::RSHRN2_vec;
            }
            return;
        case 18:
            if (!Q) {
                inst.id = instructionID::SQSHRN_vec;
            } else {
                inst.id = instructionID::SQSHRN2_vec;
            }
            return;
        case 19:
            if (!Q) {
                inst.id = instructionID::SQRSHRN_vec;
            } else {
                inst.id = instructionID::SQRSHRN2_vec;
            }
            return;
        case 20:
            if (!Q) {
                inst.id = instructionID::SSHLL_vec;
            } else {
                inst.id = instructionID::SSHLL2_vec;
            }
            return;
        case 28:
            inst.id = instructionID::SCVTF_vec_fixed;
            return;
        case 31:
            inst.id = instructionID::FCVTZS_vec_fixed;
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        switch (opcode) {
        case 0:
            inst.id = instructionID::USHR_vec;
            return;
        case 2:
            inst.id = instructionID::USRA_vec;
            return;
        case 4:
            inst.id = instructionID::URSHR_vec;
            return;
        case 6:
            inst.id = instructionID::URSRA_vec;
            return;
        case 8:
            inst.id = instructionID::SRI_vec;
            return;
        case 10:
            inst.id = instructionID::SLI_vec;
            return;
        case 12:
            inst.id = instructionID::SQSHLU_vec;
            return;
        case 14:
            inst.id = instructionID::UQSHL_vec_imm;
            return;
        case 16:
            if (!Q) {
                inst.id = instructionID::SQSHRUN_vec;
            } else {
                inst.id = instructionID::SQSHRUN2_vec;
            }
            return;
        case 17:
            if (!Q) {
                inst.id = instructionID::SQRSHRUN_vec;
            } else {
                inst.id = instructionID::SQRSHRUN2_vec;
            }
            return;
        case 18:
            if (!Q) {
                inst.id = instructionID::UQSHRN_vec;
            } else {
                inst.id = instructionID::UQSHRN2_vec;
            }
            return;
        case 19:
            if (!Q) {
                inst.id = instructionID::UQRSHRN_vec;
            } else {
                inst.id = instructionID::UQRSHRN2_vec;
            }
            return;
        case 20:
            if (!Q) {
                inst.id = instructionID::USHLL_vec;
            } else {
                inst.id = instructionID::USHLL2_vec;
            }
            return;
        case 28:
            inst.id = instructionID::UCVTF_vec_fixed;
            return;
        case 31:
            inst.id = instructionID::FCVTZU_vec_fixed;
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeSIMDVectorXIndexed(uint32_t bits, DecodedInstruction& inst) {
    bool Q = testBit<30>(bits);
    bool U = testBit<29>(bits);
    auto size = bitRange<22, 23>(bits);
    auto opcode = bitRange<12, 15>(bits);

    if (!U) {
        switch (opcode) {
        case 1:
            if (size > 1) {
                inst.id = instructionID::FMLA_vec_element;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 2:
            if (!Q) {
                inst.id = instructionID::SMLAL_vec_element;
            } else {
                inst.id = instructionID::SMLAL2_vec_element;
            }
            return;
        case 3:
            if (!Q) {
                inst.id = instructionID::SQDMLAL_vec_element;
            } else {
                inst.id = instructionID::SQDMLAL2_vec_element;
            }
            return;
        case 5:
            if (size > 1) {
                inst.id = instructionID::FMLS_vec_element;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 6:
            if (!Q) {
                inst.id = instructionID::SMLSL_vec_element;
            } else {
                inst.id = instructionID::SMLSL2_vec_element;
            }
            return;
        case 7:
            if (!Q) {
                inst.id = instructionID::SQDMLSL_vec_element;
            } else {
                inst.id = instructionID::SQDMLSL2_vec_element;
            }
            return;
        case 8:
            inst.id = instructionID::MUL_vec_element;
            return;
        case 9:
            if (size > 1) {
                inst.id = instructionID::FMUL_vec_element;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 10:
            if (!Q) {
                inst.id = instructionID::SMULL_vec_element;
            } else {
                inst.id = instructionID::SMULL2_vec_element;
            }
            return;
        case 11:
            if (!Q) {
                inst.id = instructionID::SQDMULL_vec_element;
            } else {
                inst.id = instructionID::SQDMULL2_vec_element;
            }
            return;
        case 12:
            inst.id = instructionID::SQDMULH_vec_element;
            return;
        case 13:
            inst.id = instructionID::SQRDMULH_vec_element;
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        switch (opcode) {
        case 0:
            inst.id = instructionID::MLA_vec_element;
            return;
        case 2:
            if (!Q) {
                inst.id = instructionID::UMLAL_vec_element;
            } else {
                inst.id = instructionID::UMLAL2_vec_element;
            }
            return;
        case 4:
            inst.id = instructionID::MLS_vec_element;
            return;
        case 6:
            if (!Q) {
                inst.id = instructionID::UMLSL_vec_element;
            } else {
                inst.id = instructionID::UMLSL2_vec_element;
            }
            return;
        case 9:
            if (size > 1) {
                inst.id = instructionID::FMULX_vec_element;
            } else {
                inst.id = instructionID::INVALID;
            }
            return;
        case 10:
            if (!Q) {
                inst.id = instructionID::UMULL_vec_element;
            } else {
                inst.id = instructionID::UMULL2_vec_element;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeSIMD(uint32_t bits, DecodedInstruction& inst) {
    auto op0 = bitRange<28, 31>(bits);
    auto op1 = bitRange<23, 24>(bits);
    auto op2 = bitRange<19, 22>(bits);
    auto op3 = bitRange<17, 18>(bits);
    auto op4 = bitRange<10, 15>(bits);

    if ((op0 == 4) && (op1 < 2) && ((op2 & 0b0111) == 5) && (op3 == 0) && ((op4 & 0b000011) == 2)) {
        decodeCryptographicAES(bits, inst);
    } else if ((op0 == 5) && (op1 < 2) && !testBit<2>(op2) && ((op4 & 0b100011) == 0)) {
        decodeCryptographic3Reg(bits, inst);
    } else if ((op0 == 5) && (op1 < 2) && ((op2 & 0b0111) == 5) && (op3 == 0) && ((op4 & 0b000011) == 2)) {
        decodeCryptographic2Reg(bits, inst);
    } else if (((op0 & 0b1101) == 5) && (op1 == 0) && (op2 < 4) && ((op4 & 0b100001) == 1)) {
        decodeSIMDScalarCopy(bits, inst);
    } else if (((op0 & 0b1101) == 5) && (op1 < 2) && ((op2 & 0b0111) == 4) && (op3 == 0) && ((op4 & 0b000011) == 2)) {
        decodeSIMDScalar2Reg(bits, inst);
    } else if (((op0 & 0b1101) == 5) && (op1 < 2) && ((op2 & 0b0111) == 6) && (op3 == 0) && ((op4 & 0b000011) == 2)) {
        decodeSIMDScalarPairwise(bits, inst);
    } else if (((op0 & 0b1101) == 5) && (op1 < 2) && ((op2 & 0b0100) == 4) && ((op4 & 0b000011) == 0)) {
        decodeSIMDScalar3Different(bits, inst);
    } else if (((op0 & 0b1101) == 5) && (op1 < 2) && ((op2 & 0b0100) == 4) && ((op4 & 0b000001) == 1)) {
        decodeSIMDScalar3Same(bits, inst);
    } else if (((op0 & 0b1101) == 5) && (op1 == 2) && ((op4 & 0b000001) == 1)) {
        decodeSIMDScalarShiftImmediate(bits, inst);
    } else if (((op0 & 0b1101) == 5) && (op1 > 1) && ((op4 & 0b000001) == 0)) {
        decodeSIMDScalarXIndexed(bits, inst);
    } else if (((op0 & 0b1011) == 0) && (op1 < 2) && ((op2 & 0b0100) == 0) && ((op4 & 0b100011) == 0)) {
        decodeSIMDTableLookup(bits, inst);
    } else if (((op0 & 0b1011) == 0) && (op1 < 2) && ((op2 & 0b0100) == 0) && ((op4 & 0b100011) == 2)) {
        decodeSIMDPermute(bits, inst);
    } else if (((op0 & 0b1011) == 2) && (op1 < 2) && ((op2 & 0b0100) == 0) && ((op4 & 0b100001) == 0)) {
        decodeSIMDExtract(bits, inst);
    } else if (((op0 & 0b1001) == 0) && (op1 == 0) && ((op2 & 0b1100) == 0) && ((op4 & 0b100001) == 1)) {
        decodeSIMDCopy(bits, inst);
    } else if (((op0 & 0b1001) == 0) && (op1 < 2) && ((op2 & 0b0111) == 4) && (op3 == 0) && ((op4 & 0b000011) == 2)) {
        decodeSIMD2Reg(bits, inst);
    } else if (((op0 & 0b1001) == 0) && (op1 < 2) && ((op2 & 0b0111) == 6) && (op3 == 0) && ((op4 & 0b000011) == 2)) {
        decodeSIMDAcrossLanes(bits, inst);
    } else if (((op0 & 0b1001) == 0) && (op1 < 2) && ((op2 & 0b0100) == 4) && ((op4 & 0b000011) == 0)) {
        decodeSIMD3Different(bits, inst);
    } else if (((op0 & 0b1001) == 0) && (op1 < 2) && ((op2 & 0b0100) == 4) && ((op4 & 0b000001) == 1)) {
        decodeSIMD3Same(bits, inst);
    } else if (((op0 & 0b1001) == 0) && (op1 == 2) && (op2 == 0) && ((op4 & 0b000001) == 1)) {
        decodeSIMDModifiedImmediate(bits, inst);
    } else if (((op0 & 0b1001) == 0) && (op1 == 2) && (op2 != 0) && ((op4 & 0b000001) == 1)) {
        decodeSIMDShiftImmediate(bits, inst);
    } else if (((op0 & 0b1001) == 0) && (op1 > 1) && ((op4 & 0b000001) == 0)) {
        decodeSIMDVectorXIndexed(bits, inst);
    } else {
        inst.id = instructionID::INVALID;
    }
}

} // namespace cpu::decoder