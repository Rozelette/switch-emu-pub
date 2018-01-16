#include "decode.h"
#include "bitutils.h"
#include "unreachable.h"
#include "log.h"

namespace cpu::decoder {

void decodePCRel(uint32_t bits, DecodedInstruction& inst) {
    bool isPage = testBit<31>(bits);

    if (isPage) {
        inst.id = instructionID::ADRP;
    } else {
        inst.id = instructionID::ADR;
    }
}

void decodeAddSubImm(uint32_t bits, DecodedInstruction& inst) {
    auto shift = bitRange<22, 23>(bits);
    if (shift > 1) {
        inst.id = instructionID::INVALID;
        return;
    }

    // b31 - is64
    // b30 - isSub
    // b29 - updatesFlags
    auto flags = bitRange<29, 31>(bits);
    switch (flags) {
    case 0:
        inst.id = instructionID::ADD_32_imm;
        break;
    case 1:
        inst.id = instructionID::ADDS_32_imm;
        break;
    case 2:
        inst.id = instructionID::SUB_32_imm;
        break;
    case 3:
        inst.id = instructionID::SUBS_32_imm;
        break;
    case 4:
        inst.id = instructionID::ADD_64_imm;
        break;
    case 5:
        inst.id = instructionID::ADDS_64_imm;
        break;
    case 6:
        inst.id = instructionID::SUB_64_imm;
        break;
    case 7:
        inst.id = instructionID::SUBS_64_imm;
        break;
    }
}

void decodeLogicalImm(uint32_t bits, DecodedInstruction& inst) {
    bool is64 = testBit<31>(bits);
    auto opc = bitRange<29, 30>(bits);
    auto N = bitRange<22, 22>(bits);

    if (!is64 && (N == 1)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (is64) {
        switch (opc) {
        case 0:
            // AND (immediate)
            inst.id = instructionID::AND_64_imm;
            break;
        case 1:
            // ORR (immediate)
            inst.id = instructionID::ORR_64_imm;
            break;
        case 2:
            // EOR (immediate)
            inst.id = instructionID::EOR_64_imm;
            break;
        case 3:
            // ANDS (immediate)
            inst.id = instructionID::ANDS_64_imm;
            break;
        }
    } else {
        switch (opc) {
        case 0:
            // AND (immediate)
            inst.id = instructionID::AND_32_imm;
            break;
        case 1:
            // ORR (immediate)
            inst.id = instructionID::ORR_32_imm;
            break;
        case 2:
            // EOR (immediate)
            inst.id = instructionID::EOR_32_imm;
            break;
        case 3:
            // ANDS (immediate)
            inst.id = instructionID::ANDS_32_imm;
            break;
        }
    }
}

void decodeMoveWide(uint32_t bits, DecodedInstruction& inst) {
    bool is64 = testBit<31>(bits);
    auto opc = bitRange<29, 30>(bits);
    auto hw = bitRange<21, 22>(bits);
    if (((!is64) && (hw > 1)) || (opc == 1)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (is64) {
        switch (opc) {
        case 0:
            // MOVN
            inst.id = instructionID::MOVN_64;
            break;
        case 2:
            // MOVZ
            inst.id = instructionID::MOVZ_64;
            break;
        case 3:
            // MOVK
            inst.id = instructionID::MOVK_64;
            break;
        }
    } else {
        switch (opc) {
        case 0:
            // MOVN
            inst.id = instructionID::MOVN_32;
            break;
        case 2:
            // MOVZ
            inst.id = instructionID::MOVZ_32;
            break;
        case 3:
            // MOVK
            inst.id = instructionID::MOVK_32;
            break;
        }
    }
}

void decodeBitfield(uint32_t bits, DecodedInstruction& inst) {
    auto sf = bitRange<31, 31>(bits);
    auto opc = bitRange<29, 30>(bits);
    auto N = bitRange<22, 22>(bits);
    auto immr = bitRange<16, 21>(bits);
    auto imms = bitRange<10, 15>(bits);

    if ((sf != N) || (opc == 3)) {
        inst.id = instructionID::INVALID;
        return;
    }

    if ((sf == 0) && ((immr > 31) || (imms > 31))) {
        inst.id = instructionID::INVALID;
        return;
    }

    if (sf == 1) {
        switch (opc) {
        case 0:
            // SBFM
            inst.id = instructionID::SBFM_64;
            break;
        case 1:
            // BFM
            inst.id = instructionID::BFM_64;
            break;
        case 2:
            // UBFM
            inst.id = instructionID::UBFM_64;
            break;
        }
    } else {
        // sf == 0
        switch (opc) {
        case 0:
            // SBFM
            inst.id = instructionID::SBFM_32;
            break;
        case 1:
            // BFM
            inst.id = instructionID::BFM_32;
            break;
        case 2:
            // UBFM
            inst.id = instructionID::UBFM_32;
            break;
        }
    }
}

void decodeExtract(uint32_t bits, DecodedInstruction& inst) {
    auto sf = bitRange<31, 31>(bits);
    auto op21 = bitRange<29, 30>(bits);
    auto N = bitRange<22, 22>(bits);
    auto o0 = bitRange<21, 21>(bits);
    auto imms = bitRange<10, 15>(bits);

    if ((sf == 0) && (op21 == 0) && (N == 0) && (o0 == 0) && (imms < 32)) {
        inst.id = instructionID::EXTR_32;
    } else if ((sf == 1) && (op21 == 0) && (N == 1) && (o0 == 0)) {
        inst.id = instructionID::EXTR_64;
    } else {
        inst.id = instructionID::INVALID;
    }
}

void decodeDPImm(uint32_t bits, DecodedInstruction& inst) {
    auto opcode = bitRange<23, 25>(bits);

    switch (opcode) {
    case 0:case 1:
    {
        // 00x - PC-rel. addressing
        decodePCRel(bits, inst);
        break;
    }
    case 2:case 3:
    {
        // 01x - Add/subtract (immediate)
        decodeAddSubImm(bits, inst);
        break;
    }
    case 4:
    {
        // 100 - Logical (immediate)
        decodeLogicalImm(bits, inst);
        break;
    }
    case 5:
    {
        // 101 - Move wide (immediate)
        decodeMoveWide(bits, inst);
        break;
    }
    case 6:
    {
        // 110 - Bitfield
        decodeBitfield(bits, inst);
        break;
    }
    case 7:
    {
        // 111 - Extract
        decodeExtract(bits, inst);
        break;
    }
    default:
        UNREACHABLE();
    }
}

} // namespace cpu::decoder