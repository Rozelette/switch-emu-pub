#include "decode.h"
#include "bitutils.h"
#include "unreachable.h"
#include "log.h"

namespace cpu::decoder {

void decodeSystem(uint32_t bits, DecodedInstruction& inst) {
    auto L = bitRange<21, 21>(bits);
    auto op0 = bitRange<19, 20>(bits);
    auto op1 = bitRange<16, 18>(bits);
    auto CRn = bitRange<12, 15>(bits);
    auto CRm = bitRange<8, 11>(bits);
    auto op2 = bitRange<5, 7>(bits);
    auto Rt = bitRange<0, 4>(bits);

    if (L == 1) {
        if (op0 == 0) {
            // 00 - Unallocated
            inst.id = instructionID::INVALID;
        } else if (op0 == 1) {
            // 01 - SYSL
            inst.id = instructionID::SYSL;
        } else {
            // 1x - MRS
            inst.id = instructionID::MRS;
        }
    } else {
        // L == 0
        if (op0 == 0) {
            // 00
            switch (CRn) {
            case 0:case 1:case 5:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:case 14:case 15:
                // 000x & 0101 & 011x & 1xxx - Unallocated
                inst.id = instructionID::INVALID;
                break;
            case 4:
                // 0100
                if (Rt == 31) {
                    // 11111 - MSR (immediate)
                    inst.id = instructionID::MSR_imm;
                } else {
                    // != 11111
                    inst.id = instructionID::INVALID;
                }
                break;
            case 2:
                // 0010
                if (op1 != 3) {
                    // xx0 & x0x & 1xx - Unallocated
                    inst.id = instructionID::INVALID;
                    return;
                }
                if (Rt != 31) {
                    inst.id = instructionID::INVALID;
                    return;
                }

                if (CRm != 0) {
                    // != 0000 - HINT - 8 to 127 variant
                    inst.id = instructionID::HINT;
                } else {
                    // 0000
                    switch (op2) {
                    case 0:
                        // 000 - NOP
                        inst.id = instructionID::NOP;
                        break;
                    case 1:
                        // 001 - YIELD
                        inst.id = instructionID::YIELD;
                        break;
                    case 2:
                        // 010 - WFE
                        inst.id = instructionID::WFE;
                        break;
                    case 3:
                        // 011 - WFI
                        inst.id = instructionID::WFI;
                        break;
                    case 4:
                        // 100 - SEV
                        inst.id = instructionID::SEV;
                        break;
                    case 5:
                        // 101 - SEVL
                        inst.id = instructionID::SEVL;
                        break;
                    case 6:case 7:
                        // 11x - HINT - 6 and 7 variant
                        inst.id = instructionID::HINT;
                        break;
                    }
                }
                break;
            case 3:
                // 0011
                if (op1 != 3) {
                    // xx0 & x0x & 1xx - Unallocated
                    inst.id = instructionID::INVALID;
                    return;
                }
                if (Rt != 31) {
                    inst.id = instructionID::INVALID;
                    return;
                }

                switch (op2) {
                case 0:case 1:case 3:case 7:
                    // 000 & 001 & 011 & 111 - Unallocated
                    inst.id = instructionID::INVALID;
                    break;
                case 2:
                    //  010
                    inst.id = instructionID::CLREX;
                    break;
                case 4:
                    // 100
                    inst.id = instructionID::DSB;
                    break;
                case 5:
                    // 101
                    inst.id = instructionID::DMB;
                    break;
                case 6:
                    // 110
                    inst.id = instructionID::ISB;
                    break;
                }
                break;
            }
        } else if (op0 == 1) {
            // 01 - SYS
            inst.id = instructionID::SYS;
        } else {
            // 1x - MSR (register)
            inst.id = instructionID::MSR_reg;
        }
    }
}

void decodeExceptionGenerating(uint32_t bits, DecodedInstruction& inst) {
    auto opc = bitRange<21, 23>(bits);
    auto op2 = bitRange<2, 4>(bits);
    auto LL = bitRange<0, 1>(bits);

    if (op2 != 0) {
        inst.id = instructionID::INVALID;
        return;
    }
    switch (opc) {
    case 0:
        // 000
        switch (LL) {
        case 0:
            // 00
            inst.id = instructionID::INVALID;
            break;
        case 1:
            // 01 - SVC
            inst.id = instructionID::SVC;
            break;
        case 2:
            // 10 - HVC
            inst.id = instructionID::HVC;
            break;
        case 3:
            // 11 - SMC
            inst.id = instructionID::SMC;
            break;
        }
        break;
    case 1:
        // 001
        if (LL == 0) {
            // 00 - BRK
            inst.id = instructionID::BRK;
        } else {
            inst.id = instructionID::INVALID;
        }
        break;
    case 2:
        // 010
        if (LL == 0) {
            // 00 - HLT
            inst.id = instructionID::HLT;
        } else {
            inst.id = instructionID::INVALID;
        }
        break;
    case 5:
        // 101
        switch (LL) {
        case 0:
            // 00
            inst.id = instructionID::INVALID;
            break;
        case 1:
            // 01 - DCPS1
            inst.id = instructionID::DCPS1;
            break;
        case 2:
            // 10 - DCPS2
            inst.id = instructionID::DCPS2;
            break;
        case 3:
            // 11 - DCPS3
            inst.id = instructionID::DCPS3;
            break;
        }
        break;
    case 3:case 4:case 6:case 7:
        // 011 & 100 & 11x - Unallocated
        inst.id = instructionID::INVALID;
        break;
    }
}

} // namespace cpu::decoder