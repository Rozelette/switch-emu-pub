#include <stdio.h>
#include <stdint.h>

#include "cpu.h"
#include "bitutils.h"
#include "instutils.h"
#include "interpreter.h"

namespace cpu {

namespace interpreter {

void condBranch(Core* core, AArch64Inst inst) {
    Condition cond = static_cast<Condition>(inst.cond);
    uint64_t off = signExtend<21>(static_cast<uint64_t>(inst._5_23 << 2));

    if (testCondition(core, cond)) {
        core->nextPC = core->PC + off;
    }
}

template <bool link>
void uncondBranchRegGeneric(Core* core, AArch64Inst inst) {
    uint64_t addr = core->GPRegs[inst.Rn];
    if constexpr (link) {
        core->GPRegs[30] = core->PC + 4;
    }

    core->nextPC = addr;
}

void uncondBranchRegNoLink(Core* core, AArch64Inst inst) {
    return uncondBranchRegGeneric<false>(core, inst);
}

void uncondBranchRegLink(Core* core, AArch64Inst inst) {
    return uncondBranchRegGeneric<true>(core, inst);
}

template <bool link>
void uncondBranchImmGeneric(Core* core, AArch64Inst inst) {
    uint64_t off = signExtend<28>(static_cast<uint64_t>(inst._0_25 << 2));

    if constexpr (link) {
        core->GPRegs[30] = core->PC + 4;
    }

    core->nextPC = core->PC + off;
}

void uncondBranchImmNoLink(Core* core, AArch64Inst inst) {
    return uncondBranchImmGeneric<false>(core, inst);
}

void uncondBranchImmLink(Core* core, AArch64Inst inst) {
    return uncondBranchImmGeneric<true>(core, inst);
}

template<bool is64, bool negate>
void compareBranchGeneric(Core* core, AArch64Inst inst) {
    auto off = signExtend<21>(static_cast<uint64_t>(inst._5_23 << 2));

    uint64_t operand = core->GPRegs[inst.Rt];
    if constexpr (!is64) {
        operand &= bitmask<32, uint64_t>();
    }

    bool cond = (operand == 0);
    if constexpr (negate) {
        cond = !cond;
    }

    if (cond) {
        core->nextPC = core->PC + off;
    }
}

void cbz32(Core* core, AArch64Inst inst) {
    return compareBranchGeneric<false, false>(core, inst);
}

void cbnz32(Core* core, AArch64Inst inst) {
    return compareBranchGeneric<false, true>(core, inst);
}

void cbz64(Core* core, AArch64Inst inst) {
    return compareBranchGeneric<true, false>(core, inst);
}

void cbnz64(Core* core, AArch64Inst inst) {
    return compareBranchGeneric<true, true>(core, inst);
}

template<bool negate>
void testBitBranchGeneric(Core* core, AArch64Inst inst) {
    auto off = signExtend<16>(static_cast<uint64_t>(inst._5_18 << 2));

    uint64_t operand = core->GPRegs[inst.Rt];
    uint64_t bit = inst.b40 | (inst.b5 << 5);

    bool cond = testBit(operand, bit);
    if constexpr (negate) {
        cond = !cond;
    }

    if (cond) {
        core->nextPC = core->PC + off;
    }
}

void tbz(Core* core, AArch64Inst inst) {
    return testBitBranchGeneric<true>(core, inst);
}

void tbnz(Core* core, AArch64Inst inst) {
    return testBitBranchGeneric<false>(core, inst);
}

void registerBranchInstructions() {
    registerInstruction(instructionID::B_cond, condBranch);
    registerInstruction(instructionID::BR, uncondBranchRegNoLink);
    registerInstruction(instructionID::BLR, uncondBranchRegLink);
    registerInstruction(instructionID::RET, uncondBranchRegNoLink);
    registerInstruction(instructionID::ERET, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::DRPS, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::B, uncondBranchImmNoLink);
    registerInstruction(instructionID::BL, uncondBranchImmLink);
    registerInstruction(instructionID::CBZ_32, cbz32);
    registerInstruction(instructionID::CBNZ_32, cbnz32);
    registerInstruction(instructionID::CBZ_64, cbz64);
    registerInstruction(instructionID::CBNZ_64, cbnz64);
    registerInstruction(instructionID::TBZ, tbz);
    registerInstruction(instructionID::TBNZ, tbnz);
}

} // namespace interpreter

} // namespace cpu