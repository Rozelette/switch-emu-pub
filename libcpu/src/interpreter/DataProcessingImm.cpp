#include <stdio.h>
#include <functional>

#include "cpu.h"
#include "bitutils.h"
#include "instutils.h"
#include "interpreter.h"

namespace cpu {

namespace interpreter {

template<bool is64, bool isSub, bool updatesFlags>
void addSubImmGeneric(Core* core, AArch64Inst inst) {
    bool shift = testBit<22>(inst.bits);
    uint64_t imm = static_cast<uint64_t>(inst._10_21) << ((shift)? 12 : 0);

    uint64_t operand1 = (inst.Rn == 31)? core->SP : core->GPRegs[inst.Rn];
    uint64_t operand2 = ((isSub)? ~imm : imm);

    uint64_t result = addWithCarry<is64, updatesFlags>(operand1, operand2, isSub ? 1 : 0, core->PSTATE);

    if constexpr (!is64) {
        result &= bitmask<32, uint64_t>();
    }

    if (inst.Rd == 31) {
        if constexpr (!updatesFlags) {
            core->SP = result;
        }
    } else {
        core->GPRegs[inst.Rd] = result;
    }
}

void add32Imm(Core* core, AArch64Inst inst) {
    return addSubImmGeneric<false, false, false>(core, inst);
}

void adds32Imm(Core* core, AArch64Inst inst) {
    return addSubImmGeneric<false, false, true>(core, inst);
}

void sub32Imm(Core* core, AArch64Inst inst) {
    return addSubImmGeneric<false, true, false>(core, inst);
}

void subs32Imm(Core* core, AArch64Inst inst) {
    return addSubImmGeneric<false, true, true>(core, inst);
}

void add64Imm(Core* core, AArch64Inst inst) {
    return addSubImmGeneric<true, false, false>(core, inst);
}

void adds64Imm(Core* core, AArch64Inst inst) {
    return addSubImmGeneric<true, false, true>(core, inst);
}

void sub64Imm(Core* core, AArch64Inst inst) {
    return addSubImmGeneric<true, true, false>(core, inst);
}

void subs64Imm(Core* core, AArch64Inst inst) {
    return addSubImmGeneric<true, true, true>(core, inst);
}

// TODO split opc into template?
template<bool is64>
void moveWideImmGeneric(Core* core, AArch64Inst inst) {
    uint64_t shift = inst.hw * 16;

    uint64_t imm = static_cast<uint64_t>(inst._5_20) << shift;

    if (inst.opc == 0) {
        // MOVN
        imm = ~imm;
    } else if (inst.opc == 3) {
        // MOVK
        uint64_t old = core->GPRegs[inst.Rd];
        uint64_t mask = ~(0xFFFFllu << shift);
        old &= mask;
        imm = old | imm;
    }
    // do nothing else for MOVZ (opc == 2)

    if constexpr (!is64) {
        imm &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = imm;
}

void mov32(Core* core, AArch64Inst inst) {
    return moveWideImmGeneric<false>(core, inst);
}

void mov64(Core* core, AArch64Inst inst) {
    return moveWideImmGeneric<true>(core, inst);
}

template<bool isPage>
void PCRelativeGeneric(Core* core, AArch64Inst inst) {
    uint64_t imm = (inst.immhi << 2) | inst.immlo;
    imm = signExtend<21>(imm);

    uint64_t base = core->PC;
    if constexpr (isPage) {
        imm <<= 12;
        base &= 0xFFFFFFFFFFFFF000;
    }

    core->GPRegs[inst.Rd] = base + imm;
}

void adr(Core* core, AArch64Inst inst) {
    return PCRelativeGeneric<false>(core, inst);
}

void adrp(Core* core, AArch64Inst inst) {
    return PCRelativeGeneric<true>(core, inst);
}

template<bool is64, bool updatesFlags>
void logicalGeneric(Core* core, AArch64Inst inst, std::function<uint64_t(uint64_t, uint64_t)> func) {
    uint64_t imm = getLogicalImmediate(inst);

    uint64_t op1 = core->GPRegs[inst.Rn];
    uint64_t res = func(op1, imm);

    if constexpr (!is64) {
        res &= bitmask<32, uint64_t>();
    }

    if (inst.Rd == 31) {
        // ANDS does not write to SP
        if constexpr (!updatesFlags) {
            core->SP = res;
        }
    } else {
        core->GPRegs[inst.Rd] = res;
    }

    if constexpr (updatesFlags) {
        core->PSTATE.N = testBit<(is64)? 63 : 31>(res);
        core->PSTATE.Z = (res == 0);
        core->PSTATE.C = false;
        core->PSTATE.V = false;
    }
}

void and32Imm(Core* core, AArch64Inst inst) {
    return logicalGeneric<false, false>(core, inst, std::bit_and<uint64_t>());
}

void orr32Imm(Core* core, AArch64Inst inst) {
    return logicalGeneric<false, false>(core, inst, std::bit_or<uint64_t>());
}

void eor32Imm(Core* core, AArch64Inst inst) {
    return logicalGeneric<false, false>(core, inst, std::bit_xor<uint64_t>());
}

void ands32Imm(Core* core, AArch64Inst inst) {
    return logicalGeneric<false, true>(core, inst, std::bit_and<uint64_t>());
}

void and64Imm(Core* core, AArch64Inst inst) {
    return logicalGeneric<true, false>(core, inst, std::bit_and<uint64_t>());
}

void orr64Imm(Core* core, AArch64Inst inst) {
    return logicalGeneric<true, false>(core, inst, std::bit_or<uint64_t>());
}

void eor64Imm(Core* core, AArch64Inst inst) {
    return logicalGeneric<true, false>(core, inst, std::bit_xor<uint64_t>());
}

void ands64Imm(Core* core, AArch64Inst inst) {
    return logicalGeneric<true, true>(core, inst, std::bit_and<uint64_t>());
}

// TODO clean up
template<bool is64, bool writeZeros, bool isSigned>
void bitfieldMoveGeneric(Core* core, AArch64Inst inst) {
    uint64_t data = core->GPRegs[inst.Rn];
    if constexpr (!is64) {
        data &= bitmask<32, uint64_t>();
    }

    bool originalSignBit [[maybe_unused]] = testBit(data, inst.imms);

    uint64_t finalPos = (inst.imms + ((is64) ? 64 : 32) - inst.immr) % ((is64) ? 64 : 32);

    uint64_t sourceMask = bitmask(static_cast<uint64_t>(inst.imms + 1));

    if (inst.imms == 63) {
        // TODO avoid
        sourceMask = 0xFFFFFFFFFFFFFFFF;
    }
    data &= sourceMask;

    // ROR
    data = (data >> inst.immr) | (data << (((is64) ? 64 : 32) - inst.immr));

    uint64_t shiftedMask = (finalPos > inst.imms) ? sourceMask << (finalPos - inst.imms) : sourceMask >> (inst.imms - finalPos);

    data &= shiftedMask;

    if constexpr (writeZeros) {
        if constexpr (isSigned) {
            if (originalSignBit) {
                // TODO do this correctly (it might already be)
                // say we move 0001xxxx to 0yyyyy00, it should be 11xxxx00 (I think)
                data = signExtend(data, finalPos);
            }
        }
    } else {
        // TODO this is probably wrong
        uint64_t oldData = core->GPRegs[inst.Rd];
        uint64_t rotatedMask = (sourceMask >> inst.immr) | (sourceMask << (((is64) ? 64 : 32) - inst.immr));
        oldData &= ~rotatedMask;
        data |= oldData;
    }

    if constexpr (!is64) {
        data &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = data;
}

void sbfm32(Core* core, AArch64Inst inst) {
    return bitfieldMoveGeneric<false, true, true>(core, inst);
}

void bfm32(Core* core, AArch64Inst inst) {
    return bitfieldMoveGeneric<false, false, false>(core, inst);
}

void ubfm32(Core* core, AArch64Inst inst) {
    return bitfieldMoveGeneric<false, true, false>(core, inst);
}

void sbfm64(Core* core, AArch64Inst inst) {
    return bitfieldMoveGeneric<true, true, true>(core, inst);
}

void bfm64(Core* core, AArch64Inst inst) {
    return bitfieldMoveGeneric<true, false, false>(core, inst);
}

void ubfm64(Core* core, AArch64Inst inst) {
    return bitfieldMoveGeneric<true, true, false>(core, inst);
}

template<bool is64>
void extractGeneric(Core* core, AArch64Inst inst) {
    uint64_t operand1 = core->GPRegs[inst.Rn];
    uint64_t operand2 = core->GPRegs[inst.Rm];

    if constexpr (!is64) {
        operand1 &= bitmask<32, uint64_t>();
        operand2 &= bitmask<32, uint64_t>();
    }

    uint64_t result = (operand2 >> inst.imms) | (operand1 << (((is64)? 64 : 32) - inst.imms));


    if constexpr (!is64) {
        result &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = result;
}

void extr32(Core* core, AArch64Inst inst) {
    return extractGeneric<false>(core, inst);
}

void extr64(Core* core, AArch64Inst inst) {
    return extractGeneric<true>(core, inst);
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
    registerInstruction(instructionID::AND_32_imm, and32Imm);
    registerInstruction(instructionID::ORR_32_imm, orr32Imm);
    registerInstruction(instructionID::EOR_32_imm, eor32Imm);
    registerInstruction(instructionID::ANDS_32_imm, ands32Imm);
    registerInstruction(instructionID::AND_64_imm, and64Imm);
    registerInstruction(instructionID::ORR_64_imm, orr64Imm);
    registerInstruction(instructionID::EOR_64_imm, eor64Imm);
    registerInstruction(instructionID::ANDS_64_imm, ands64Imm);
    registerInstruction(instructionID::MOVN_32, mov32);
    registerInstruction(instructionID::MOVZ_32, mov32);
    registerInstruction(instructionID::MOVK_32, mov32);
    registerInstruction(instructionID::MOVN_64, mov64);
    registerInstruction(instructionID::MOVZ_64, mov64);
    registerInstruction(instructionID::MOVK_64, mov64);
    registerInstruction(instructionID::SBFM_32, sbfm32);
    registerInstruction(instructionID::BFM_32, bfm32);
    registerInstruction(instructionID::UBFM_32, ubfm32);
    registerInstruction(instructionID::SBFM_64, sbfm64);
    registerInstruction(instructionID::BFM_64, bfm64);
    registerInstruction(instructionID::UBFM_64, ubfm64);
    registerInstruction(instructionID::EXTR_32, extr32);
    registerInstruction(instructionID::EXTR_64, extr64);
}

} // namespace interpreter

} // namespace cpu