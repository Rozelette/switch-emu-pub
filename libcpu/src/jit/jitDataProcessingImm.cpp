#include "jit.h"
#include "bitutils.h"

namespace cpu {

namespace jit {

template<bool is64, bool isSub, bool updatesFlags>
void addSubImmGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    bool shift = testBit<22>(inst.bits);
    uint64_t imm = static_cast<uint64_t>(inst._10_21) << ((shift) ? 12 : 0);
    uint64_t operand2 = ((isSub) ? ~imm: imm);
    uint64_t carry = (isSub) ? 1 : 0;

    // TODO can we avoid this? Does operand2 = 0xFFFFFFFF, carry = 1 cause problems?
    uint64_t operand2WithCarry = operand2 + carry;
    if (!is64) {
        operand2WithCarry &= bitmask<32, uint64_t>();
    }

    // Ignores SP alignment
    auto operand1 = (inst.Rn == 31)? a.loadGpRegisterRead(a.coreSPMem) : a.loadGpRegisterRead(a.coreGPRegs[inst.Rn]);
    auto result = (inst.Rd == 31) ? ((!updatesFlags) ? a.loadGpRegisterWrite(a.coreSPMem) : a.allocGpTemp(8)) : a.loadGpRegisterWrite(a.coreGPRegs[inst.Rd]);
    auto temp = a.allocGpTemp(operand1);

    if (is64) {
        a.add(temp, operand2WithCarry);
        a.mov(result, temp);
    } else {
        a.add(temp.r32(), operand2WithCarry);
        a.mov(result.r32(), temp.r32());
    }
    if (updatesFlags) {
        auto op1ForFlags = a.allocGpTemp(operand1);
        auto op2ForFlags = a.allocGpTemp(8, operand2);
        // TODO pull this logic out somewhere
        a.test(temp, temp);
        a.sete(a.coreZflagMem);

        // reduce to MSB
        a.shr(temp, (is64) ? 63 : 31);
        a.shr(op1ForFlags, (is64) ? 63 : 31);
        a.shr(op2ForFlags, (is64) ? 63 : 31);
        a.and_(temp, 1); // TODO needed?
        a.and_(op1ForFlags, 1); // TODO needed?
        a.and_(op2ForFlags, 1); // TODO needed?

        a.mov(a.coreNflagMem, temp.r8());

        // AND r64, imm64 does not exist, have to use reg for mask
        auto mask = a.allocGpTemp(8, (is64) ? bitmask<63, uint64_t>() : bitmask<31, uint64_t>());
        auto partial = a.allocGpTemp(operand1);
        auto scratch = a.allocGpTemp(8, operand2);

        a.and_(partial, mask);
        a.and_(scratch, mask);
        a.add(partial, scratch);
        a.add(partial, carry);
        a.shr(partial, (is64) ? 63 : 31);
        a.and_(partial, 1); // TODO needed?

        // Set C if (op1MSB && op2MSB) || (op1MSB && parMSB) || (op2MSB && parMSB)
        // Essentially, if 2 or more MSBs are set
        a.mov(scratch, partial);
        a.add(scratch, op1ForFlags);
        a.add(scratch, op2ForFlags);
        a.cmp(scratch, 2);
        a.setge(a.coreCflagMem);

        // Set V if (op1MSB == op2MSB) && (op1MSB != resMSB)
        a.cmp(op1ForFlags, op2ForFlags);
        a.sete(scratch);
        a.xor_(op1ForFlags, temp);
        a.and_(scratch, op1ForFlags);
        a.mov(a.coreVflagMem, scratch.r8());
    }
}

void add32Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubImmGeneric<false, false, false>(a, inst);
}

void adds32Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubImmGeneric<false, false, true>(a, inst);
}

void sub32Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubImmGeneric<false, true, false>(a, inst);
}

void subs32Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubImmGeneric<false, true, true>(a, inst);
}

void add64Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubImmGeneric<true, false, false>(a, inst);
}

void adds64Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubImmGeneric<true, false, true>(a, inst);
}

void sub64Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubImmGeneric<true, true, false>(a, inst);
}

void subs64Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    return addSubImmGeneric<true, true, true>(a, inst);
}

template<bool is64>
void moveWideImmGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    uint64_t shift = inst.hw * 16;

    uint64_t imm = static_cast<uint64_t>(inst._5_20) << shift;

    auto dst = a.loadGpRegisterReadWrite(a.coreGPRegs[inst.Rd]);
    if (inst.opc == 0) {
        // MOVN
        imm = ~imm;

        if (is64) {
            a.mov(dst, imm);
        } else {
            a.mov(dst.r32(), imm);
        }
    } else if (inst.opc == 3) {
        // MOVK
        // AND/OR r64, imm64 does not exist, have to use reg for mask
        uint64_t mask = ~(0xFFFFllu << shift);
        if (!is64) {
            mask &= bitmask<32, uint64_t>();
        }
        auto tempReg = a.allocGpTemp(8, mask);
        a.and_(dst, tempReg);
        a.mov(tempReg, imm);
        a.or_(dst, tempReg);
    } else {
        // MOVZ (opc == 2)
        if (is64) {
            a.mov(dst, imm);
        } else {
            a.mov(dst.r32(), imm);
        }
    }
}

void mov32(AArch64EmuAssembler& a, AArch64Inst inst) {
    return moveWideImmGeneric<false>(a, inst);
}

void mov64(AArch64EmuAssembler& a, AArch64Inst inst) {
    return moveWideImmGeneric<true>(a, inst);
}

template<bool isPage>
void PCRelativeGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    uint64_t imm = (inst.immhi << 2) | inst.immlo;
    imm = signExtend<21>(imm);

    uint64_t base = a.currentPC;
    if (isPage) {
        imm <<= 12;
        base &= 0xFFFFFFFFFFFFF000;
    }

    auto dst = a.loadGpRegisterWrite(a.coreGPRegs[inst.Rd]);
    a.mov(dst, base + imm);
}

void adr(AArch64EmuAssembler& a, AArch64Inst inst) {
    return PCRelativeGeneric<false>(a, inst);
}

void adrp(AArch64EmuAssembler& a, AArch64Inst inst) {
    return PCRelativeGeneric<true>(a, inst);
}

void registerDPImmInstructions() {
    registerInstruction(instructionID::ADR, adr);
    registerInstruction(instructionID::ADRP, adrp);
    registerInstruction(instructionID::ADD_32_imm, add32Imm);
    registerInstruction(instructionID::ADDS_32_imm, adds32Imm);
    registerInstruction(instructionID::SUB_32_imm, sub32Imm);
    registerInstruction(instructionID::SUBS_32_imm, subs32Imm);
    registerInstruction(instructionID::ADD_64_imm, add64Imm);
    registerInstruction(instructionID::ADDS_64_imm, adds64Imm);
    registerInstruction(instructionID::SUB_64_imm, sub64Imm);
    registerInstruction(instructionID::SUBS_64_imm, subs64Imm);
    registerInstruction(instructionID::AND_32_imm, FALLBACK);
    registerInstruction(instructionID::ORR_32_imm, FALLBACK);
    registerInstruction(instructionID::EOR_32_imm, FALLBACK);
    registerInstruction(instructionID::ANDS_32_imm, FALLBACK);
    registerInstruction(instructionID::AND_64_imm, FALLBACK);
    registerInstruction(instructionID::ORR_64_imm, FALLBACK);
    registerInstruction(instructionID::EOR_64_imm, FALLBACK);
    registerInstruction(instructionID::ANDS_64_imm, FALLBACK);
    registerInstruction(instructionID::MOVN_32, mov32);
    registerInstruction(instructionID::MOVZ_32, mov32);
    registerInstruction(instructionID::MOVK_32, mov32);
    registerInstruction(instructionID::MOVN_64, mov64);
    registerInstruction(instructionID::MOVZ_64, mov64);
    registerInstruction(instructionID::MOVK_64, mov64);
    registerInstruction(instructionID::SBFM_32, FALLBACK);
    registerInstruction(instructionID::BFM_32, FALLBACK);
    registerInstruction(instructionID::UBFM_32, FALLBACK);
    registerInstruction(instructionID::SBFM_64, FALLBACK);
    registerInstruction(instructionID::BFM_64, FALLBACK);
    registerInstruction(instructionID::UBFM_64, FALLBACK);
    registerInstruction(instructionID::EXTR_32, FALLBACK);
    registerInstruction(instructionID::EXTR_64, FALLBACK);
}

} // namespace jit

} // namespace cpu