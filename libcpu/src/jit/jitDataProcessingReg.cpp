#include "jit.h"
#include "bitutils.h"
#include "unreachable.h"

namespace cpu {

namespace jit {

template<bool is64, bool negate, bool updatesFlags>
void logicalShiftedRegGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    SHIFT_TYPE shiftType = static_cast<SHIFT_TYPE>(inst.shift);

    auto operand1 = a.loadGpRegisterRead(a.coreGPRegs[inst.Rn]);
    auto operand2 = a.loadGpRegisterRead(a.coreGPRegs[inst.Rm]);

    auto temp1 = a.allocGpTemp(operand1);
    auto temp2 = a.allocGpTemp(operand2);

    if (inst._10_15 != 0) {
        switch (shiftType) {
        case LSL:
            a.shl(temp2, inst._10_15);
            break;
        case LSR:
            // TODO does this work for 32bit? do the higher bits get in the way?
            a.shr(temp2, inst._10_15);
            break;
        case ASR:
            // TODO, unsure about 64/32bit
            break;
        case ROR:
            // TODO, unsure about 64/32bit
            break;
        default:
            break;
        }
    }

    if (negate) {
        a.neg(temp2);
    }

    switch (bitRange<29, 30>(inst.bits)) {
    case 0:
    case 3:
        a.and_(temp1, temp2);
        break;
    case 1:
        a.or_(temp1, temp2);
        break;
    case 2:
        a.xor_(temp1, temp2);
        break;
    default:
        UNREACHABLE();
    }

    auto result = a.loadGpRegisterWrite(a.coreGPRegs[inst.Rd]);

    if (is64) {
        a.mov(result, temp1);
    } else {
        a.mov(result.r32(), temp1.r32());
    }


    if (updatesFlags) {
        // TODO pull this logic out somewhere
        a.test(temp1, temp1);
        a.sete(a.coreZflagMem);

        // TODO is the and really needed?
        a.shr(temp1, (is64) ? 63 : 31);
        a.and_(temp1, 1);
        a.mov(a.coreNflagMem, temp1.r8());

        a.mov(a.coreCflagMem, 0);
        a.mov(a.coreVflagMem, 0);
    }
}

void and32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, false, false>(a, inst);
}

void bic32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, true, false>(a, inst);
}

void orr32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, false, false>(a, inst);
}

void orn32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, true, false>(a, inst);
}

void eor32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, false, false>(a, inst);
}

void eon32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, true, false>(a, inst);
}

void ands32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, false, true>(a, inst);
}

void bics32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, true, true>(a, inst);
}

void and64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, false, false>(a, inst);
}

void bic64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, true, false>(a, inst);
}

void orr64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, false, false>(a, inst);
}

void orn64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, true, false>(a, inst);
}

void eor64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, false, false>(a, inst);
}

void eon64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, true, false>(a, inst);
}

void ands64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, false, true>(a, inst);
}

void bics64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, true, true>(a, inst);
}

template<bool is64, bool isSub, bool updatesFlags>
void addSubShiftedRegGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    SHIFT_TYPE shiftType = static_cast<SHIFT_TYPE>(inst.shift);

    auto operand1 = a.loadGpRegisterRead(a.coreGPRegs[inst.Rn]);
    auto operand2 = a.loadGpRegisterRead(a.coreGPRegs[inst.Rm]);

    auto temp1 = a.allocGpTemp(operand1);
    auto temp2 = a.allocGpTemp(operand2);

    switch (shiftType) {
    case LSL:
        a.shl(temp2, inst._10_15);
        break;
    case LSR:
        // TODO does this work for 32bit? do the higher bits get in the way?
        a.shr(temp2, inst._10_15);
        break;
    case ASR:
        // TODO, unsure about 64/32bit
        break;
    case ROR:
        // TODO, unsure about 64/32bit
        break;
    default:
        break;
    }

    if (isSub) {
        a.neg(temp2);
        a.inc(temp2);
    }

    auto temp = a.allocGpTemp(8);
    auto result = a.loadGpRegisterWrite(a.coreGPRegs[inst.Rd]);

    if (is64) {
        a.mov(temp, temp1);
        a.add(temp, temp2);
        a.mov(result, temp);
    } else {
        a.mov(temp.r32(), temp1.r32());
        a.add(temp.r32(), temp2.r32());
        a.mov(result.r32(), temp.r32());
    }

    if (updatesFlags) {
        // TODO pull this logic out somewhere
        a.test(temp, temp);
        a.sete(a.coreZflagMem);

        // reduce to MSB
        a.shr(temp, (is64) ? 63 : 31);
        a.shr(temp1, (is64) ? 63 : 31);
        a.shr(temp2, (is64) ? 63 : 31);
        a.and_(temp, 1);
        a.and_(temp1, 1);
        a.and_(temp2, 1);

        a.mov(a.coreNflagMem, temp.r8());

        auto scratch = a.allocGpTemp(8);

        // Set C if (op1MSB && op2MSB) || (op1MSB && resMSB) || (op2MSB && resMSB)
        // Essentially, if 2 or more MSBs are set
        a.mov(scratch, temp);
        a.add(scratch, temp1);
        a.add(scratch, temp2);
        a.cmp(scratch, 2);
        a.setge(a.coreCflagMem);

        // Set V if (op1MSB == op2MSB) && (op1MSB != resMSB)
        a.cmp(temp1, temp2);
        a.sete(scratch);
        a.xor_(temp1, temp);
        a.and_(scratch, temp1);
        a.mov(a.coreVflagMem, scratch.r8());
    }
}

void add32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubShiftedRegGeneric<false, false, false>(a, inst);
}

void adds32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubShiftedRegGeneric<false, false, true>(a, inst);
}

void sub32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubShiftedRegGeneric<false, true, false>(a, inst);
}

void subs32Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubShiftedRegGeneric<false, true, true>(a, inst);
}

void add64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubShiftedRegGeneric<true, false, false>(a, inst);
}

void adds64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubShiftedRegGeneric<true, false, true>(a, inst);
}

void sub64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubShiftedRegGeneric<true, true, false>(a, inst);
}

void subs64Shifted(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubShiftedRegGeneric<true, true, true>(a, inst);
}

void registerDPRegInstructions() {
    registerInstruction(instructionID::UDIV_32, FALLBACK);
    registerInstruction(instructionID::SDIV_32, FALLBACK);
    registerInstruction(instructionID::LSLV_32, FALLBACK);
    registerInstruction(instructionID::LSRV_32, FALLBACK);
    registerInstruction(instructionID::ASRV_32, FALLBACK);
    registerInstruction(instructionID::RORV_32, FALLBACK);
    registerInstruction(instructionID::UDIV_64, FALLBACK);
    registerInstruction(instructionID::SDIV_64, FALLBACK);
    registerInstruction(instructionID::LSLV_64, FALLBACK);
    registerInstruction(instructionID::LSRV_64, FALLBACK);
    registerInstruction(instructionID::ASRV_64, FALLBACK);
    registerInstruction(instructionID::RORV_64, FALLBACK);
    registerInstruction(instructionID::CRC32B, FALLBACK);
    registerInstruction(instructionID::CRC32H, FALLBACK);
    registerInstruction(instructionID::CRC32W, FALLBACK);
    registerInstruction(instructionID::CRC32X, FALLBACK);
    registerInstruction(instructionID::CRC32CB, FALLBACK);
    registerInstruction(instructionID::CRC32CH, FALLBACK);
    registerInstruction(instructionID::CRC32CW, FALLBACK);
    registerInstruction(instructionID::CRC32CX, FALLBACK);
    registerInstruction(instructionID::RBIT_32, FALLBACK);
    registerInstruction(instructionID::REV16_32, FALLBACK);
    registerInstruction(instructionID::REV_32, FALLBACK);
    registerInstruction(instructionID::CLZ_32, FALLBACK);
    registerInstruction(instructionID::CLS_32, FALLBACK);
    registerInstruction(instructionID::RBIT_64, FALLBACK);
    registerInstruction(instructionID::REV16_64, FALLBACK);
    registerInstruction(instructionID::REV32_64, FALLBACK);
    registerInstruction(instructionID::REV_64, FALLBACK);
    registerInstruction(instructionID::CLZ_64, FALLBACK);
    registerInstruction(instructionID::CLS_64, FALLBACK);
    registerInstruction(instructionID::AND_32_shifted, FALLBACK);
    registerInstruction(instructionID::BIC_32_shifted, FALLBACK);
    registerInstruction(instructionID::ORR_32_shifted, orr32Shifted);
    registerInstruction(instructionID::ORN_32_shifted, FALLBACK);
    registerInstruction(instructionID::EOR_32_shifted, FALLBACK);
    registerInstruction(instructionID::EON_32_shifted, FALLBACK);
    registerInstruction(instructionID::ANDS_32_shifted, FALLBACK);
    registerInstruction(instructionID::BICS_32_shifted, FALLBACK);
    registerInstruction(instructionID::AND_64_shifted, FALLBACK);
    registerInstruction(instructionID::BIC_64_shifted, FALLBACK);
    registerInstruction(instructionID::ORR_64_shifted, FALLBACK);
    registerInstruction(instructionID::ORN_64_shifted, FALLBACK);
    registerInstruction(instructionID::EOR_64_shifted, FALLBACK);
    registerInstruction(instructionID::EON_64_shifted, FALLBACK);
    registerInstruction(instructionID::ANDS_64_shifted, FALLBACK);
    registerInstruction(instructionID::BICS_64_shifted, FALLBACK);
    registerInstruction(instructionID::ADD_32_shifted, add32Shifted);
    registerInstruction(instructionID::ADDS_32_shifted, FALLBACK);
    registerInstruction(instructionID::SUB_32_shifted, FALLBACK);
    registerInstruction(instructionID::SUBS_32_shifted, FALLBACK);
    registerInstruction(instructionID::ADD_64_shifted, FALLBACK);
    registerInstruction(instructionID::ADDS_64_shifted, FALLBACK);
    registerInstruction(instructionID::SUB_64_shifted, FALLBACK);
    registerInstruction(instructionID::SUBS_64_shifted, FALLBACK);
    registerInstruction(instructionID::ADD_32_extended, FALLBACK);
    registerInstruction(instructionID::ADDS_32_extended, FALLBACK);
    registerInstruction(instructionID::SUB_32_extended, FALLBACK);
    registerInstruction(instructionID::SUBS_32_extended, FALLBACK);
    registerInstruction(instructionID::ADD_64_extended, FALLBACK);
    registerInstruction(instructionID::ADDS_64_extended, FALLBACK);
    registerInstruction(instructionID::SUB_64_extended, FALLBACK);
    registerInstruction(instructionID::SUBS_64_extended, FALLBACK);
    registerInstruction(instructionID::ADC_32, FALLBACK);
    registerInstruction(instructionID::ADCS_32, FALLBACK);
    registerInstruction(instructionID::SBC_32, FALLBACK);
    registerInstruction(instructionID::SBCS_32, FALLBACK);
    registerInstruction(instructionID::ADC_64, FALLBACK);
    registerInstruction(instructionID::ADCS_64, FALLBACK);
    registerInstruction(instructionID::SBC_64, FALLBACK);
    registerInstruction(instructionID::SBCS_64, FALLBACK);
    registerInstruction(instructionID::CCMN_32_reg, FALLBACK);
    registerInstruction(instructionID::CCMP_32_reg, FALLBACK);
    registerInstruction(instructionID::CCMN_64_reg, FALLBACK);
    registerInstruction(instructionID::CCMP_64_reg, FALLBACK);
    registerInstruction(instructionID::CCMN_32_imm, FALLBACK);
    registerInstruction(instructionID::CCMP_32_imm, FALLBACK);
    registerInstruction(instructionID::CCMN_64_imm, FALLBACK);
    registerInstruction(instructionID::CCMP_64_imm, FALLBACK);
    registerInstruction(instructionID::CSEL_32, FALLBACK);
    registerInstruction(instructionID::CSINC_32, FALLBACK);
    registerInstruction(instructionID::CSINV_32, FALLBACK);
    registerInstruction(instructionID::CSNEG_32, FALLBACK);
    registerInstruction(instructionID::CSEL_64, FALLBACK);
    registerInstruction(instructionID::CSINC_64, FALLBACK);
    registerInstruction(instructionID::CSINV_64, FALLBACK);
    registerInstruction(instructionID::CSNEG_64, FALLBACK);
    registerInstruction(instructionID::MADD_32, FALLBACK);
    registerInstruction(instructionID::MSUB_32, FALLBACK);
    registerInstruction(instructionID::MADD_64, FALLBACK);
    registerInstruction(instructionID::MSUB_64, FALLBACK);
    registerInstruction(instructionID::SMADDL, FALLBACK);
    registerInstruction(instructionID::SMSUBL, FALLBACK);
    registerInstruction(instructionID::SMULH, FALLBACK);
    registerInstruction(instructionID::UMADDL, FALLBACK);
    registerInstruction(instructionID::UMSUBL, FALLBACK);
    registerInstruction(instructionID::UMULH, FALLBACK);
}

} // namespace jit

} // namespace cpu