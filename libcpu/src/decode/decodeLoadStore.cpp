#include "decode.h"
#include "bitutils.h"
#include "unreachable.h"
#include "log.h"

namespace cpu::decoder {

void decodeLoadStoreSIMDMultipleStructs(uint32_t bits, DecodedInstruction& inst) {
    auto L = bitRange<22, 22>(bits);
    auto opcode = bitRange<12, 15>(bits);

    switch (opcode) {
    case 0:
        if (L == 0) {
            inst.id = instructionID::ST4_multi;
        } else {
            inst.id = instructionID::LD4_multi;
        }
        return;
    case 2:
        if (L == 0) {
            inst.id = instructionID::ST1_multi_4reg;
        } else {
            inst.id = instructionID::LD1_multi_4reg;
        }
        return;
    case 4:
        if (L == 0) {
            inst.id = instructionID::ST3_multi;
        } else {
            inst.id = instructionID::LD3_multi;
        }
        return;
    case 6:
        if (L == 0) {
            inst.id = instructionID::ST1_multi_3reg;
        } else {
            inst.id = instructionID::LD1_multi_3reg;
        }
        return;
    case 7:
        if (L == 0) {
            inst.id = instructionID::ST1_multi_1reg;
        } else {
            inst.id = instructionID::LD1_multi_1reg;
        }
        return;
    case 8:
        if (L == 0) {
            inst.id = instructionID::ST2_multi;
        } else {
            inst.id = instructionID::LD2_multi;
        }
        return;
    case 10:
        if (L == 0) {
            inst.id = instructionID::ST1_multi_2reg;
        } else {
            inst.id = instructionID::LD1_multi_2reg;
        }
        return;
    default:
        inst.id = instructionID::INVALID;
        return;
    }
}

void decodeLoadStoreSIMDMultipleStructsPostIndex(uint32_t bits, DecodedInstruction& inst) {
    auto L = bitRange<22, 22>(bits);
    auto Rm = bitRange<16, 20>(bits);
    auto opcode = bitRange<12, 15>(bits);

    if (Rm != 31) {
        switch (opcode) {
        case 0:
            if (L == 0) {
                inst.id = instructionID::ST4_multi_reg;
            } else {
                inst.id = instructionID::LD4_multi_reg;
            }
            return;
        case 2:
            if (L == 0) {
                inst.id = instructionID::ST1_multi_reg_4reg;
            } else {
                inst.id = instructionID::LD1_multi_reg_4reg;
            }
            return;
        case 4:
            if (L == 0) {
                inst.id = instructionID::ST3_multi_reg;
            } else {
                inst.id = instructionID::LD3_multi_reg;
            }
            return;
        case 6:
            if (L == 0) {
                inst.id = instructionID::ST1_multi_reg_3reg;
            } else {
                inst.id = instructionID::LD1_multi_reg_3reg;
            }
            return;
        case 7:
            if (L == 0) {
                inst.id = instructionID::ST1_multi_reg_1reg;
            } else {
                inst.id = instructionID::LD1_multi_reg_1reg;
            }
            return;
        case 8:
            if (L == 0) {
                inst.id = instructionID::ST2_multi_reg;
            } else {
                inst.id = instructionID::LD2_multi_reg;
            }
            return;
        case 10:
            if (L == 0) {
                inst.id = instructionID::ST1_multi_reg_2reg;
            } else {
                inst.id = instructionID::LD1_multi_reg_2reg;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    } else {
        switch (opcode) {
        case 0:
            if (L == 0) {
                inst.id = instructionID::ST4_multi_offset;
            } else {
                inst.id = instructionID::LD4_multi_offset;
            }
            return;
        case 2:
            if (L == 0) {
                inst.id = instructionID::ST1_multi_offset_4reg;
            } else {
                inst.id = instructionID::LD1_multi_offset_4reg;
            }
            return;
        case 4:
            if (L == 0) {
                inst.id = instructionID::ST3_multi_offset;
            } else {
                inst.id = instructionID::LD3_multi_offset;
            }
            return;
        case 6:
            if (L == 0) {
                inst.id = instructionID::ST1_multi_offset_3reg;
            } else {
                inst.id = instructionID::LD1_multi_offset_3reg;
            }
            return;
        case 7:
            if (L == 0) {
                inst.id = instructionID::ST1_multi_offset_1reg;
            } else {
                inst.id = instructionID::LD1_multi_offset_1reg;
            }
            return;
        case 8:
            if (L == 0) {
                inst.id = instructionID::ST2_multi_offset;
            } else {
                inst.id = instructionID::LD2_multi_offset;
            }
            return;
        case 10:
            if (L == 0) {
                inst.id = instructionID::ST1_multi_offset_2reg;
            } else {
                inst.id = instructionID::LD1_multi_offset_2reg;
            }
            return;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
    }
}

void decodeLoadStoreSIMDSingleStruct(uint32_t bits, DecodedInstruction& inst) {
    auto L = bitRange<22, 22>(bits);
    auto R = bitRange<21, 21>(bits);
    auto opcode = bitRange<13, 15>(bits);
    auto S = bitRange<12, 12>(bits);
    auto size = bitRange<10, 11>(bits);

    if (L == 0) {
        if (R == 0) {
            switch (opcode) {
            case 0:
                inst.id = instructionID::ST1_single_8;
                return;
            case 1:
                inst.id = instructionID::ST3_single_8;
                return;
            case 2:
                if ((size & 0x1) == 0) {
                    inst.id = instructionID::ST1_single_16;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 3:
                if ((size & 0x1) == 0) {
                    inst.id = instructionID::ST3_single_16;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 4:
                if (size == 0) {
                    inst.id = instructionID::ST1_single_32;
                } else if ((size == 1) && (S == 0)) {
                    inst.id = instructionID::ST1_single_64;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 5:
                if (size == 0) {
                    inst.id = instructionID::ST3_single_32;
                } else if ((size == 1) && (S == 0)) {
                    inst.id = instructionID::ST3_single_64;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            default:
                inst.id = instructionID::INVALID;
                return;
            }
        } else {
            switch (opcode) {
            case 0:
                inst.id = instructionID::ST2_single_8;
                return;
            case 1:
                inst.id = instructionID::ST4_single_8;
                return;
            case 2:
                if ((size & 0x1) == 0) {
                    inst.id = instructionID::ST2_single_16;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 3:
                if ((size & 0x1) == 0) {
                    inst.id = instructionID::ST4_single_16;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 4:
                if (size == 0) {
                    inst.id = instructionID::ST2_single_32;
                } else if ((size == 1) && (S == 0)) {
                    inst.id = instructionID::ST2_single_64;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 5:
                if (size == 0) {
                    inst.id = instructionID::ST4_single_32;
                } else if ((size == 1) && (S == 0)) {
                    inst.id = instructionID::ST4_single_64;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            default:
                inst.id = instructionID::INVALID;
                return;
            }
        }
    } else {
        if (R == 0) {
            switch (opcode) {
            case 0:
                inst.id = instructionID::LD1_single_8;
                return;
            case 1:
                inst.id = instructionID::LD3_single_8;
                return;
            case 2:
                if ((size & 0x1) == 0) {
                    inst.id = instructionID::LD1_single_16;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 3:
                if ((size & 0x1) == 0) {
                    inst.id = instructionID::LD3_single_16;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 4:
                if (size == 0) {
                    inst.id = instructionID::LD1_single_32;
                } else if ((size == 1) && (S == 0)) {
                    inst.id = instructionID::LD1_single_64;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 5:
                if (size == 0) {
                    inst.id = instructionID::LD3_single_32;
                } else if ((size == 1) && (S == 0)) {
                    inst.id = instructionID::LD3_single_64;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 6:
                if (S == 0) {
                    inst.id = instructionID::LD1R;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 7:
                if (S == 0) {
                    inst.id = instructionID::LD3R;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            default:
                inst.id = instructionID::INVALID;
                return;
            }
        } else {
            switch (opcode) {
            case 0:
                inst.id = instructionID::LD2_single_8;
                return;
            case 1:
                inst.id = instructionID::LD4_single_8;
                return;
            case 2:
                if ((size & 0x1) == 0) {
                    inst.id = instructionID::LD2_single_16;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 3:
                if ((size & 0x1) == 0) {
                    inst.id = instructionID::LD4_single_16;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 4:
                if (size == 0) {
                    inst.id = instructionID::LD2_single_32;
                } else if ((size == 1) && (S == 0)) {
                    inst.id = instructionID::LD2_single_64;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 5:
                if (size == 0) {
                    inst.id = instructionID::LD4_single_32;
                } else if ((size == 1) && (S == 0)) {
                    inst.id = instructionID::LD4_single_64;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 6:
                if (S == 0) {
                    inst.id = instructionID::LD2R;
                } else {
                    inst.id = instructionID::INVALID;
                }
                return;
            case 7:
                if (S == 0) {
                    inst.id = instructionID::LD4R;
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
}

void decodeLoadStoreSIMDSingleStructPostIndex(uint32_t bits, DecodedInstruction& inst) {
    auto L = bitRange<22, 22>(bits);
    auto R = bitRange<21, 21>(bits);
    auto Rm = bitRange<16, 20>(bits);
    auto opcode = bitRange<13, 15>(bits);
    auto S = bitRange<12, 12>(bits);
    auto size = bitRange<10, 11>(bits);

    if (Rm != 31) {
        if (L == 0) {
            if (R == 0) {
                switch (opcode) {
                case 0:
                    inst.id = instructionID::ST1_single_8_reg;
                    return;
                case 1:
                    inst.id = instructionID::ST3_single_8_reg;
                    return;
                case 2:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::ST1_single_16_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 3:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::ST3_single_16_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 4:
                    if (size == 0) {
                        inst.id = instructionID::ST1_single_32_reg;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::ST1_single_64_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 5:
                    if (size == 0) {
                        inst.id = instructionID::ST3_single_32_reg;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::ST3_single_64_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                default:
                    inst.id = instructionID::INVALID;
                    return;
                }
            } else {
                switch (opcode) {
                case 0:
                    inst.id = instructionID::ST2_single_8_reg;
                    return;
                case 1:
                    inst.id = instructionID::ST4_single_8_reg;
                    return;
                case 2:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::ST2_single_16_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 3:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::ST4_single_16_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 4:
                    if (size == 0) {
                        inst.id = instructionID::ST2_single_32_reg;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::ST2_single_64_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 5:
                    if (size == 0) {
                        inst.id = instructionID::ST4_single_32_reg;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::ST4_single_64_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                default:
                    inst.id = instructionID::INVALID;
                    return;
                }
            }
        } else {
            if (R == 0) {
                switch (opcode) {
                case 0:
                    inst.id = instructionID::LD1_single_8_reg;
                    return;
                case 1:
                    inst.id = instructionID::LD3_single_8_reg;
                    return;
                case 2:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::LD1_single_16_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 3:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::LD3_single_16_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 4:
                    if (size == 0) {
                        inst.id = instructionID::LD1_single_32_reg;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::LD1_single_64_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 5:
                    if (size == 0) {
                        inst.id = instructionID::LD3_single_32_reg;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::LD3_single_64_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 6:
                    if (S == 0) {
                        inst.id = instructionID::LD1R_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 7:
                    if (S == 0) {
                        inst.id = instructionID::LD3R_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                default:
                    inst.id = instructionID::INVALID;
                    return;
                }
            } else {
                switch (opcode) {
                case 0:
                    inst.id = instructionID::LD2_single_8_reg;
                    return;
                case 1:
                    inst.id = instructionID::LD4_single_8_reg;
                    return;
                case 2:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::LD2_single_16_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 3:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::LD4_single_16_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 4:
                    if (size == 0) {
                        inst.id = instructionID::LD2_single_32_reg;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::LD2_single_64_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 5:
                    if (size == 0) {
                        inst.id = instructionID::LD4_single_32_reg;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::LD4_single_64_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 6:
                    if (S == 0) {
                        inst.id = instructionID::LD2R_reg;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 7:
                    if (S == 0) {
                        inst.id = instructionID::LD4R_reg;
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
    } else {
        if (L == 0) {
            if (R == 0) {
                switch (opcode) {
                case 0:
                    inst.id = instructionID::ST1_single_8_imm;
                    return;
                case 1:
                    inst.id = instructionID::ST3_single_8_imm;
                    return;
                case 2:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::ST1_single_16_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 3:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::ST3_single_16_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 4:
                    if (size == 0) {
                        inst.id = instructionID::ST1_single_32_imm;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::ST1_single_64_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 5:
                    if (size == 0) {
                        inst.id = instructionID::ST3_single_32_imm;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::ST3_single_64_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                default:
                    inst.id = instructionID::INVALID;
                    return;
                }
            } else {
                switch (opcode) {
                case 0:
                    inst.id = instructionID::ST2_single_8_imm;
                    return;
                case 1:
                    inst.id = instructionID::ST4_single_8_imm;
                    return;
                case 2:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::ST2_single_16_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 3:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::ST4_single_16_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 4:
                    if (size == 0) {
                        inst.id = instructionID::ST2_single_32_imm;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::ST2_single_64_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 5:
                    if (size == 0) {
                        inst.id = instructionID::ST4_single_32_imm;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::ST4_single_64_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                default:
                    inst.id = instructionID::INVALID;
                    return;
                }
            }
        } else {
            if (R == 0) {
                switch (opcode) {
                case 0:
                    inst.id = instructionID::LD1_single_8_imm;
                    return;
                case 1:
                    inst.id = instructionID::LD3_single_8_imm;
                    return;
                case 2:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::LD1_single_16_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 3:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::LD3_single_16_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 4:
                    if (size == 0) {
                        inst.id = instructionID::LD1_single_32_imm;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::LD1_single_64_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 5:
                    if (size == 0) {
                        inst.id = instructionID::LD3_single_32_imm;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::LD3_single_64_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 6:
                    if (S == 0) {
                        inst.id = instructionID::LD1R_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 7:
                    if (S == 0) {
                        inst.id = instructionID::LD3R_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                default:
                    inst.id = instructionID::INVALID;
                    return;
                }
            } else {
                switch (opcode) {
                case 0:
                    inst.id = instructionID::LD2_single_8_imm;
                    return;
                case 1:
                    inst.id = instructionID::LD4_single_8_imm;
                    return;
                case 2:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::LD2_single_16_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 3:
                    if ((size & 0x1) == 0) {
                        inst.id = instructionID::LD4_single_16_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 4:
                    if (size == 0) {
                        inst.id = instructionID::LD2_single_32_imm;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::LD2_single_64_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 5:
                    if (size == 0) {
                        inst.id = instructionID::LD4_single_32_imm;
                    } else if ((size == 1) && (S == 0)) {
                        inst.id = instructionID::LD4_single_64_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 6:
                    if (S == 0) {
                        inst.id = instructionID::LD2R_imm;
                    } else {
                        inst.id = instructionID::INVALID;
                    }
                    return;
                case 7:
                    if (S == 0) {
                        inst.id = instructionID::LD4R_imm;
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
    }
}

void decodeLoadStoreExclusive(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<30, 31>(bits);
    auto o2 = bitRange<23, 23>(bits);
    auto L = bitRange<22, 22>(bits);
    auto o1 = bitRange<21, 21>(bits);
    auto o0 = bitRange<15, 15>(bits);

    if ((o2 == 1) && (o1 == 0) && (o0 == 0)) {
        inst.id = instructionID::INVALID;
        return;
    }
    if ((o2 == 1) && (o1 == 1)) {
        inst.id = instructionID::INVALID;
        return;
    }
    if ((size < 2) && (o1 == 1)) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (size) {
    case 0:
        if (o2 == 0) {
            if (L == 0) {
                if (o0 == 0) {
                    inst.id = instructionID::STXRB;
                } else {
                    // o0 == 1
                    inst.id = instructionID::STLXRB;
                }
            } else {
                // L == 1
                if (o0 == 0) {
                    inst.id = instructionID::LDXRB;
                } else {
                    // o0 == 1
                    inst.id = instructionID::LDAXRB;
                }
            }
        } else {
            // o2 == 1
            if (L == 0) {
                inst.id = instructionID::STLRB;
            } else {
                // L == 1
                inst.id = instructionID::LDARB;
            }
        }
        break;
    case 1:
        if (o2 == 0) {
            if (L == 0) {
                if (o0 == 0) {
                    inst.id = instructionID::STXRH;
                } else {
                    // o0 == 1
                    inst.id = instructionID::STLXRH;
                }
            } else {
                // L == 1
                if (o0 == 0) {
                    inst.id = instructionID::LDXRH;
                } else {
                    // o0 == 1
                    inst.id = instructionID::LDAXRH;
                }
            }
        } else {
            // o2 == 1
            if (L == 0) {
                inst.id = instructionID::STLRH;
            } else {
                // L == 1
                inst.id = instructionID::LDARH;
            }
        }
        break;
    case 2:
        if (o2 == 0) {
            if (o1 == 0) {
                if (L == 0) {
                    if (o0 == 0) {
                        inst.id = instructionID::STXR32;
                    } else {
                        // o0 == 1
                        inst.id = instructionID::STLXR32;
                    }
                } else {
                    // L == 1
                    if (o0 == 0) {
                        inst.id = instructionID::LDXR32;
                    } else {
                        // o0 == 1
                        inst.id = instructionID::LDAXR32;
                    }
                }
            } else {
                // o1 == 1
                if (L == 0) {
                    if (o0 == 0) {
                        inst.id = instructionID::STXP32;
                    } else {
                        // o0 == 1
                        inst.id = instructionID::STLXP32;
                    }
                } else {
                    // L == 1
                    if (o0 == 0) {
                        inst.id = instructionID::LDXP32;
                    } else {
                        // o0 == 1
                        inst.id = instructionID::LDAXP32;
                    }
                }
            }
        } else {
            // o2 == 1
            if (L == 0) {
                inst.id = instructionID::STLR32;
            } else {
                // L == 1
                inst.id = instructionID::LDAR32;
            }
        }
        break;
    case 3:
        if (o2 == 0) {
            if (o1 == 0) {
                if (L == 0) {
                    if (o0 == 0) {
                        inst.id = instructionID::STXR64;
                    } else {
                        // o0 == 1
                        inst.id = instructionID::STLXR64;
                    }
                } else {
                    // L == 1
                    if (o0 == 0) {
                        inst.id = instructionID::LDXR64;
                    } else {
                        // o0 == 1
                        inst.id = instructionID::LDAXR64;
                    }
                }
            } else {
                // o1 == 1
                if (L == 0) {
                    if (o0 == 0) {
                        inst.id = instructionID::STXP64;
                    } else {
                        // o0 == 1
                        inst.id = instructionID::STLXP64;
                    }
                } else {
                    // L == 1
                    if (o0 == 0) {
                        inst.id = instructionID::LDXP64;
                    } else {
                        // o0 == 1
                        inst.id = instructionID::LDAXP64;
                    }
                }
            }
        } else {
            // o2 == 1
            if (L == 0) {
                inst.id = instructionID::STLR64;
            } else {
                // L == 1
                inst.id = instructionID::LDAR64;
            }
        }
        break;
    default:
        UNREACHABLE();
    }
}

void decodeLoadStoreUnprivileged(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<30, 31>(bits);
    bool isSIMD = testBit<26>(bits);
    auto opc = bitRange<22, 23>(bits);

    if (isSIMD) {
        inst.id = instructionID::INVALID;
        return;
    }

    switch (size) {
    case 0:
        switch (opc) {
        case 0:
            inst.id = instructionID::STTRB;
            break;
        case 1:
            inst.id = instructionID::LDTRB;
            break;
        case 2:
            inst.id = instructionID::LDTRSB64;
            break;
        case 3:
            inst.id = instructionID::LDTRSB32;
            break;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
        return;
    case 1:
        switch (opc) {
        case 0:
            inst.id = instructionID::STTRH;
            break;
        case 1:
            inst.id = instructionID::LDTRH;
            break;
        case 2:
            inst.id = instructionID::LDTRSH64;
            break;
        case 3:
            inst.id = instructionID::LDTRSH32;
            break;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
        return;
    case 2:
        switch (opc) {
        case 0:
            inst.id = instructionID::STTR32;
            break;
        case 1:
            inst.id = instructionID::LDTR32;
            break;
        case 2:
            inst.id = instructionID::LDTRSW;
            break;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
        return;
    case 3:
        switch (opc) {
        case 0:
            inst.id = instructionID::STTR64;
            break;
        case 1:
            inst.id = instructionID::LDTR64;
            break;
        default:
            inst.id = instructionID::INVALID;
            return;
        }
        return;
    default:
        inst.id = instructionID::INVALID;
        return;
    }
}

void decodeLoadStoreRegUnsignedImm(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<30, 31>(bits);
    bool isSIMD = testBit<26>(bits);
    auto opc = bitRange<22, 23>(bits);

    if ((size != 0) && isSIMD && (opc > 1)) {
        inst.id = instructionID::INVALID;
    }

    if (isSIMD) {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR8_imm_float;
                break;
            case 1:
                inst.id = instructionID::LDR8_imm_float;
                break;
            case 2:
                inst.id = instructionID::STR128_imm;
                break;
            case 3:
                inst.id = instructionID::LDR128_imm;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR16_imm_float;
                break;
            case 1:
                inst.id = instructionID::LDR16_imm_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR32_imm_float;
                break;
            case 1:
                inst.id = instructionID::LDR32_imm_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR64_imm_float;
                break;
            case 1:
                inst.id = instructionID::LDR64_imm_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    } else {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STRB_imm;
                break;
            case 1:
                inst.id = instructionID::LDRB_imm;
                break;
            case 2:
                inst.id = instructionID::LDRSB64_imm;
                break;
            case 3:
                inst.id = instructionID::LDRSB32_imm;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STRH_imm;
                break;
            case 1:
                inst.id = instructionID::LDRH_imm;
                break;
            case 2:
                inst.id = instructionID::LDRSH64_imm;
                break;
            case 3:
                inst.id = instructionID::LDRSH32_imm;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR32_imm;
                break;
            case 1:
                inst.id = instructionID::LDR32_imm;
                break;
            case 2:
                inst.id = instructionID::LDRSW_imm;
                break;
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR64_imm;
                break;
            case 1:
                inst.id = instructionID::LDR64_imm;
                break;
            case 2:
                inst.id = instructionID::PRFM_imm;
                break;
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    }
}

void decodeLoadStoreRegImmPostIndex(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<30, 31>(bits);
    bool isSIMD = testBit<26>(bits);
    auto opc = bitRange<22, 23>(bits);

    if ((size != 0) && isSIMD && (opc > 1)) {
        inst.id = instructionID::INVALID;
    }

    if (isSIMD) {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR8_post_float;
                break;
            case 1:
                inst.id = instructionID::LDR8_post_float;
                break;
            case 2:
                inst.id = instructionID::STR128_post;
                break;
            case 3:
                inst.id = instructionID::LDR128_post;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR16_post_float;
                break;
            case 1:
                inst.id = instructionID::LDR16_post_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR32_post_float;
                break;
            case 1:
                inst.id = instructionID::LDR32_post_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR64_post_float;
                break;
            case 1:
                inst.id = instructionID::LDR64_post_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    } else {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STRB_post;
                break;
            case 1:
                inst.id = instructionID::LDRB_post;
                break;
            case 2:
                inst.id = instructionID::LDRSB64_post;
                break;
            case 3:
                inst.id = instructionID::LDRSB32_post;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STRH_post;
                break;
            case 1:
                inst.id = instructionID::LDRH_post;
                break;
            case 2:
                inst.id = instructionID::LDRSH64_post;
                break;
            case 3:
                inst.id = instructionID::LDRSH32_post;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR32_post;
                break;
            case 1:
                inst.id = instructionID::LDR32_post;
                break;
            case 2:
                inst.id = instructionID::LDRSW_post;
                break;
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR64_post;
                break;
            case 1:
                inst.id = instructionID::LDR64_post;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    }
}

void decodeLoadStoreRegImmPreIndex(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<30, 31>(bits);
    bool isSIMD = testBit<26>(bits);
    auto opc = bitRange<22, 23>(bits);

    if ((size != 0) && isSIMD && (opc > 1)) {
        inst.id = instructionID::INVALID;
    }

    if (isSIMD) {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR8_pre_float;
                break;
            case 1:
                inst.id = instructionID::LDR8_pre_float;
                break;
            case 2:
                inst.id = instructionID::STR128_pre;
                break;
            case 3:
                inst.id = instructionID::LDR128_pre;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR16_pre_float;
                break;
            case 1:
                inst.id = instructionID::LDR16_pre_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR32_pre_float;
                break;
            case 1:
                inst.id = instructionID::LDR32_pre_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR64_pre_float;
                break;
            case 1:
                inst.id = instructionID::LDR64_pre_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    } else {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STRB_pre;
                break;
            case 1:
                inst.id = instructionID::LDRB_pre;
                break;
            case 2:
                inst.id = instructionID::LDRSB64_pre;
                break;
            case 3:
                inst.id = instructionID::LDRSB32_pre;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STRH_pre;
                break;
            case 1:
                inst.id = instructionID::LDRH_pre;
                break;
            case 2:
                inst.id = instructionID::LDRSH64_pre;
                break;
            case 3:
                inst.id = instructionID::LDRSH32_pre;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR32_pre;
                break;
            case 1:
                inst.id = instructionID::LDR32_pre;
                break;
            case 2:
                inst.id = instructionID::LDRSW_pre;
                break;
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR64_pre;
                break;
            case 1:
                inst.id = instructionID::LDR64_pre;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    }
}

void decodeLoadStoreRegUnscaled(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<30, 31>(bits);
    bool isSIMD = testBit<26>(bits);
    auto opc = bitRange<22, 23>(bits);

    if ((size != 0) && isSIMD && (opc > 1)) {
        inst.id = instructionID::INVALID;
    }

    if (isSIMD) {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STUR8_float;
                break;
            case 1:
                inst.id = instructionID::LDUR8_float;
                break;
            case 2:
                inst.id = instructionID::STUR128;
                break;
            case 3:
                inst.id = instructionID::LDUR128;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STUR16_float;
                break;
            case 1:
                inst.id = instructionID::LDUR16_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STUR32_float;
                break;
            case 1:
                inst.id = instructionID::LDUR32_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STUR64_float;
                break;
            case 1:
                inst.id = instructionID::LDUR64_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    } else {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STURB;
                break;
            case 1:
                inst.id = instructionID::LDURB;
                break;
            case 2:
                inst.id = instructionID::LDURSB64;
                break;
            case 3:
                inst.id = instructionID::LDURSB32;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STURH;
                break;
            case 1:
                inst.id = instructionID::LDURH;
                break;
            case 2:
                inst.id = instructionID::LDURSH64;
                break;
            case 3:
                inst.id = instructionID::LDURSH32;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STUR32;
                break;
            case 1:
                inst.id = instructionID::LDUR32;
                break;
            case 2:
                inst.id = instructionID::LDURSW;
                break;
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STUR64;
                break;
            case 1:
                inst.id = instructionID::LDUR64;
                break;
            case 2:
                inst.id = instructionID::PRFM_unscaled;
                break;
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    }
}


void decodeLoadStorePair(uint32_t bits, DecodedInstruction& inst) {
    auto op3 = bitRange<23, 24>(bits);
    auto opc = bitRange<30, 31>(bits);
    bool isSIMD = testBit<26>(bits);
    bool isLoad = testBit<22>(bits);

    if ((opc == 3) || (!isLoad && !isSIMD && (opc == 1))) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (isSIMD) {
        switch (op3) {
        case 0:
            // Load/store no-allocate pair (offset)
            if (opc == 0) {
                if (isLoad) {
                    inst.id = instructionID::LDNP32_float;
                } else {
                    inst.id = instructionID::STNP32_float;
                }
            } else if (opc == 1) {
                if (isLoad) {
                    inst.id = instructionID::LDNP64_float;
                } else {
                    inst.id = instructionID::STNP64_float;
                }
            } else {
                // opc == 2
                if (isLoad) {
                    inst.id = instructionID::LDNP128;
                } else {
                    inst.id = instructionID::STNP128;
                }
            }
            break;
        case 1:
            // Post-index
            if (opc == 0) {
                if (isLoad) {
                    inst.id = instructionID::LDP32_post_float;
                } else {
                    inst.id = instructionID::STP32_post_float;
                }
            } else if (opc == 1) {
                if (isLoad) {
                    inst.id = instructionID::LDP64_post_float;
                } else {
                    inst.id = instructionID::STP64_post_float;
                }
            } else {
                // opc == 2
                if (isLoad) {
                    inst.id = instructionID::LDP128_post;
                } else {
                    inst.id = instructionID::STP128_post;
                }
            }
            break;
        case 2:
            // Signed offset
            if (opc == 0) {
                if (isLoad) {
                    inst.id = instructionID::LDP32_offset_float;
                } else {
                    inst.id = instructionID::STP32_offset_float;
                }
            } else if (opc == 1) {
                if (isLoad) {
                    inst.id = instructionID::LDP64_offset_float;
                } else {
                    inst.id = instructionID::STP64_offset_float;
                }
            } else {
                // opc == 2
                if (isLoad) {
                    inst.id = instructionID::LDP128_offset;
                } else {
                    inst.id = instructionID::STP128_offset;
                }
            }
            break;
        case 3:
            // Pre-index
            if (opc == 0) {
                if (isLoad) {
                    inst.id = instructionID::LDP32_pre_float;
                } else {
                    inst.id = instructionID::STP32_pre_float;
                }
            } else if (opc == 1) {
                if (isLoad) {
                    inst.id = instructionID::LDP64_pre_float;
                } else {
                    inst.id = instructionID::STP64_pre_float;
                }
            } else {
                // opc == 2
                if (isLoad) {
                    inst.id = instructionID::LDP128_pre;
                } else {
                    inst.id = instructionID::STP128_pre;
                }
            }
            break;
        default:
            UNREACHABLE();
        }
    } else {
        switch (op3) {
        case 0:
            // Load/store no-allocate pair (offset)
            if (opc == 0) {
                if (isLoad) {
                    inst.id = instructionID::LDNP32;
                } else {
                    inst.id = instructionID::STNP32;
                }
            } else if (opc == 1) {
                inst.id = instructionID::INVALID;
            } else {
                // opc == 2
                if (isLoad) {
                    inst.id = instructionID::LDNP64;
                } else {
                    inst.id = instructionID::STNP64;
                }
            }
            break;
        case 1:
            // Post-index
            if (opc == 0) {
                if (isLoad) {
                    inst.id = instructionID::LDP32_post;
                } else {
                    inst.id = instructionID::STP32_post;
                }
            } else if (opc == 1) {
                inst.id = instructionID::LDPSW_post;
            } else {
                // opc == 2
                if (isLoad) {
                    inst.id = instructionID::LDP64_post;
                } else {
                    inst.id = instructionID::STP64_post;
                }
            }
            break;
        case 2:
            // Signed offset
            if (opc == 0) {
                if (isLoad) {
                    inst.id = instructionID::LDP32_offset;
                } else {
                    inst.id = instructionID::STP32_offset;
                }
            } else if (opc == 1) {
                inst.id = instructionID::LDPSW_offset;
            } else {
                // opc == 2
                if (isLoad) {
                    inst.id = instructionID::LDP64_offset;
                } else {
                    inst.id = instructionID::STP64_offset;
                }
            }
            break;
        case 3:
            // Pre-index
            if (opc == 0) {
                if (isLoad) {
                    inst.id = instructionID::LDP32_pre;
                } else {
                    inst.id = instructionID::STP32_pre;
                }
            } else if (opc == 1) {
                inst.id = instructionID::LDPSW_pre;
            } else {
                // opc == 2
                if (isLoad) {
                    inst.id = instructionID::LDP64_pre;
                } else {
                    inst.id = instructionID::STP64_pre;
                }
            }
            break;
        default:
            UNREACHABLE();
        }
    }
}

void decodeLoadStoreRegOff(uint32_t bits, DecodedInstruction& inst) {
    auto size = bitRange<30, 31>(bits);
    bool isSIMD = testBit<26>(bits);
    auto opc = bitRange<22, 23>(bits);
    auto option = bitRange<13, 15>(bits);

    if ((size != 0) && isSIMD && (opc > 1)) {
        inst.id = instructionID::INVALID;
    }

    if (testBit<1>(option)) {
        // x0x - Unallocated
        inst.id = instructionID::INVALID;
    }

    if (isSIMD) {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR8_reg_float;
                break;
            case 1:
                inst.id = instructionID::LDR8_reg_float;
                break;
            case 2:
                inst.id = instructionID::STR128_reg;
                break;
            case 3:
                inst.id = instructionID::LDR128_reg;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR16_reg_float;
                break;
            case 1:
                inst.id = instructionID::LDR16_reg_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR32_reg_float;
                break;
            case 1:
                inst.id = instructionID::LDR32_reg_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR64_reg_float;
                break;
            case 1:
                inst.id = instructionID::LDR64_reg_float;
                break;
            case 2:
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    } else {
        switch (size) {
        case 0:
            switch (opc) {
            case 0:
                inst.id = instructionID::STRB_reg;
                break;
            case 1:
                inst.id = instructionID::LDRB_reg;
                break;
            case 2:
                inst.id = instructionID::LDRSB64_reg;
                break;
            case 3:
                inst.id = instructionID::LDRSB32_reg;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 1:
            switch (opc) {
            case 0:
                inst.id = instructionID::STRH_reg;
                break;
            case 1:
                inst.id = instructionID::LDRH_reg;
                break;
            case 2:
                inst.id = instructionID::LDRSH64_reg;
                break;
            case 3:
                inst.id = instructionID::LDRSH32_reg;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 2:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR32_reg;
                break;
            case 1:
                inst.id = instructionID::LDR32_reg;
                break;
            case 2:
                inst.id = instructionID::LDRSW_reg;
                break;
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        case 3:
            switch (opc) {
            case 0:
                inst.id = instructionID::STR64_reg;
                break;
            case 1:
                inst.id = instructionID::LDR64_reg;
                break;
            case 2:
                inst.id = instructionID::PRFM_reg;
                break;
            case 3:
                inst.id = instructionID::INVALID;
                break;
            default:
                UNREACHABLE();
            }
            break;
        default:
            UNREACHABLE();
        }
    }
}

void decodeLoadRegLiteral(uint32_t bits, DecodedInstruction& inst) {
    auto opc = bitRange<30, 31>(bits);
    bool isSIMD = testBit<26>(bits);

    if (isSIMD) {
        switch (opc) {
        case 0:
            inst.id = instructionID::LDR32_literal_float;
            break;
        case 1:
            inst.id = instructionID::LDR64_literal_float;
            break;
        case 2:
            inst.id = instructionID::LDR128_literal;
            break;
        case 3:
            inst.id = instructionID::INVALID;
            break;
        default:
            UNREACHABLE();
        }
    } else {
        switch (opc) {
        case 0:
            inst.id = instructionID::LDR32_literal;
            break;
        case 1:
            inst.id = instructionID::LDR64_literal;
            break;
        case 2:
            inst.id = instructionID::LDRSW_literal;
            break;
        case 3:
            inst.id = instructionID::PRFM_literal;
            break;
        default:
            UNREACHABLE();
        }
    }
}

void decodeLoadStore(uint32_t bits, DecodedInstruction& inst) {
    auto op0 = bitRange<31, 31>(bits);
    auto op1 = bitRange<28, 29>(bits);
    auto op2 = bitRange<26, 26>(bits);
    auto op3 = bitRange<23, 24>(bits);
    auto op4 = bitRange<16, 21>(bits);
    auto op5 = bitRange<10, 11>(bits);

    switch (op1) {
    case 0:
        // 00
        if (op2 == 1) {
            if (op0 == 1) {
                // Unallocated
                inst.id = instructionID::INVALID;
            } else {
                // op0 == 0
                switch (op3) {
                case 0:
                    if (op4 == 0) {
                        // 000000 - Advanced SIMD load/store multiple structures on page C4-203
                        decodeLoadStoreSIMDMultipleStructs(bits, inst);
                    } else {
                        // x1xxxx & xx1xxx & xxx1xx & xxxx1x & xxxxx1 - Unallocated
                        inst.id = instructionID::INVALID;
                    }
                    break;
                case 1:
                    if (op4 < 32) {
                        // 0xxxxx - Advanced SIMD load/store multiple structures (post-indexed)
                        decodeLoadStoreSIMDMultipleStructsPostIndex(bits, inst);
                    } else {
                        // 1xxxxx - Unallocated
                        inst.id = instructionID::INVALID;
                    }
                    break;
                case 2:
                    if ((op4 == 0) || (op4 == 32)) {
                        // x00000 - Advanced SIMD load/store single structure
                        decodeLoadStoreSIMDSingleStruct(bits, inst);
                    } else {
                        // x1xxxx & xx1xxx & xxx1xx & xxxx1x & xxxxx1 - Unallocated
                        inst.id = instructionID::INVALID;
                    }
                    break;
                case 3:
                    // 11 - Advanced SIMD load/store single structure (post-indexed)
                    decodeLoadStoreSIMDSingleStructPostIndex(bits, inst);
                    break;
                default:
                    UNREACHABLE();
                }
            }
        } else {
            // op2 == 0
            if (op3 < 2) {
                // 0x - Load/store exclusive
                decodeLoadStoreExclusive(bits, inst);
            } else {
                // 1x - Unallocated
                inst.id = instructionID::INVALID;
            }
        }
        break;
    case 1:
        // 01
        if (op3 < 2) {
            // 0x - Load register (literal)
            decodeLoadRegLiteral(bits, inst);
        } else {
            // 1x - Unallocated
            inst.id = instructionID::INVALID;
        }
        break;
    case 2:
        // 10
        decodeLoadStorePair(bits, inst);
        break;
    case 3:
        // 11
        if (op3 < 2) {
            // 0x
            if (op4 < 32) {
                // 0xxxxx
                switch (op5) {
                case 0:
                    // 00 - Load/store register (unscaled immediate)
                    decodeLoadStoreRegUnscaled(bits, inst);
                    break;
                case 1:
                    // 01 - Load/store register (immediate post-indexed)
                    decodeLoadStoreRegImmPostIndex(bits, inst);
                    break;
                case 2:
                    // 10 - Load/store register (unprivileged)
                    decodeLoadStoreUnprivileged(bits, inst);
                    break;
                case 3:
                    // 11 - Load/store register (immediate pre-indexed)
                    decodeLoadStoreRegImmPreIndex(bits, inst);
                    break;
                default:
                    UNREACHABLE();
                }
            } else {
                // 1xxxxx
                if (op5 == 2) {
                    // 10 - Load/store register (register offset)
                    decodeLoadStoreRegOff(bits, inst);
                } else {
                    // != 10 - Unallocated
                    inst.id = instructionID::INVALID;
                }
            }
        } else {
            // 1x - Load/store register (unsigned immediate)
            decodeLoadStoreRegUnsignedImm(bits, inst);
        }
        break;
    default:
        UNREACHABLE();
    }
}

} // namespace cpu::decoder