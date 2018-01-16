#include <stdio.h>
#include <functional>

#include "cpu.h"
#include "bitutils.h"
#include "instutils.h"
#include "interpreter.h"

namespace cpu {

namespace interpreter {

template<bool is64, bool negate, bool updatesFlags>
void logicalShiftedRegGeneric(Core* core, AArch64Inst inst, std::function<uint64_t(uint64_t, uint64_t)> func) {
    SHIFT_TYPE shiftType = static_cast<SHIFT_TYPE>(inst.shift);
    uint64_t operand1 = core->GPRegs[inst.Rn];
    uint64_t operand2 = core->GPRegs[inst.Rm];

    // TODO does this work for 32bit? do the higher bits get in the way for LSR?
    operand2 = shift(operand2, shiftType, inst._10_15, (is64)? 64 : 32);

    if constexpr (negate) {
        operand2 = ~operand2;
    }

    uint64_t result = func(operand1, operand2);

    if constexpr (!is64) {
        result &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = result;

    if constexpr (updatesFlags) {
        core->PSTATE.N = testBit<(is64)? 63 : 31>(result);
        core->PSTATE.Z = (result == 0);
        core->PSTATE.C = false;
        core->PSTATE.V = false;
    }
}

void and32Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, false, false>(core, inst, std::bit_and<uint64_t>());
}

void bic32Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, true, false>(core, inst, std::bit_and<uint64_t>());
}

void orr32Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, false, false>(core, inst, std::bit_or<uint64_t>());
}

void orn32Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, true, false>(core, inst, std::bit_or<uint64_t>());
}

void eor32Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, false, false>(core, inst, std::bit_xor<uint64_t>());
}

void eon32Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, true, false>(core, inst, std::bit_xor<uint64_t>());
}

void ands32Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, false, true>(core, inst, std::bit_and<uint64_t>());
}

void bics32Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<false, true, true>(core, inst, std::bit_and<uint64_t>());
}

void and64Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, false, false>(core, inst, std::bit_and<uint64_t>());
}

void bic64Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, true, false>(core, inst, std::bit_and<uint64_t>());
}

void orr64Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, false, false>(core, inst, std::bit_or<uint64_t>());
}

void orn64Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, true, false>(core, inst, std::bit_or<uint64_t>());
}

void eor64Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, false, false>(core, inst, std::bit_xor<uint64_t>());
}

void eon64Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, true, false>(core, inst, std::bit_xor<uint64_t>());
}

void ands64Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, false, true>(core, inst, std::bit_and<uint64_t>());
}

void bics64Shifted(Core* core, AArch64Inst inst) {
    return logicalShiftedRegGeneric<true, true, true>(core, inst, std::bit_and<uint64_t>());
}

template<bool is64, bool isSub, bool updatesFlags>
void addSubShiftedRegGeneric(Core* core, AArch64Inst inst) {
    SHIFT_TYPE shiftType = static_cast<SHIFT_TYPE>(inst.shift);
    uint64_t operand1 = core->GPRegs[inst.Rn];
    uint64_t operand2 = core->GPRegs[inst.Rm];

    operand2 = shift(operand2, shiftType, inst._10_15, (is64)? 64 : 32);
    if constexpr (isSub) {
        operand2 = ~operand2;
    }

    uint64_t result = addWithCarry<is64, updatesFlags>(operand1, operand2, isSub? 1 : 0, core->PSTATE);

    if constexpr (!is64) {
        result &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = result;
}

void add32Shifted(Core* core, AArch64Inst inst) {
    return addSubShiftedRegGeneric<false, false, false>(core, inst);
}

void adds32Shifted(Core* core, AArch64Inst inst) {
    return addSubShiftedRegGeneric<false, false, true>(core, inst);
}

void sub32Shifted(Core* core, AArch64Inst inst) {
    return addSubShiftedRegGeneric<false, true, false>(core, inst);
}

void subs32Shifted(Core* core, AArch64Inst inst) {
    return addSubShiftedRegGeneric<false, true, true>(core, inst);
}

void add64Shifted(Core* core, AArch64Inst inst) {
    return addSubShiftedRegGeneric<true, false, false>(core, inst);
}

void adds64Shifted(Core* core, AArch64Inst inst) {
    return addSubShiftedRegGeneric<true, false, true>(core, inst);
}

void sub64Shifted(Core* core, AArch64Inst inst) {
    return addSubShiftedRegGeneric<true, true, false>(core, inst);
}

void subs64Shifted(Core* core, AArch64Inst inst) {
    addSubShiftedRegGeneric<true, true, true>(core, inst);
}

template<bool is64, bool isSub, bool updatesFlags>
void addSubExtendedRegGeneric(Core* core, AArch64Inst inst) {
    uint64_t operand1;
    if (inst.Rn == 31) {
        operand1 = core->SP;
    } else {
        operand1 = core->GPRegs[inst.Rn];
    }

    // TODO pull out
    uint64_t size = 0;
    bool isSigned = false;
    uint64_t shift = inst._10_12;
    switch (inst.option) {
    case 0: // UXTB
        size = 8;
        isSigned = false;
        break;
    case 1: // UXTH
        size = 16;
        isSigned = false;
        break;
    case 2: // UXTW
        size = 32;
        isSigned = false;
        break;
    case 3: // UXTX
        size = 64;
        isSigned = false;
        break;
    case 4: // SXTB
        size = 8;
        isSigned = true;
        break;
    case 5: // SXTH
        size = 16;
        isSigned = true;
        break;
    case 6: // SXTW
        size = 32;
        isSigned = true;
        break;
    case 7: // SXTX
        size = 64;
        isSigned = true;
        break;
    }

    uint64_t operand2 = core->GPRegs[inst.Rm];

    operand2 &= bitmask(size);
    if (isSigned) {
        operand2 = signExtend(operand2, size);
    }
    operand2 <<= shift;

    if constexpr (isSub) {
        operand2 = ~operand2;
    }

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

void add32Extended(Core* core, AArch64Inst inst) {
    return addSubExtendedRegGeneric<false, false, false>(core, inst);
}

void adds32Extended(Core* core, AArch64Inst inst) {
    return addSubExtendedRegGeneric<false, false, true>(core, inst);
}

void sub32Extended(Core* core, AArch64Inst inst) {
    return addSubExtendedRegGeneric<false, true, false>(core, inst);
}

void subs32Extended(Core* core, AArch64Inst inst) {
    return addSubExtendedRegGeneric<false, true, true>(core, inst);
}

void add64Extended(Core* core, AArch64Inst inst) {
    return addSubExtendedRegGeneric<true, false, false>(core, inst);
}

void adds64Extended(Core* core, AArch64Inst inst) {
    return addSubExtendedRegGeneric<true, false, true>(core, inst);
}

void sub64Extended(Core* core, AArch64Inst inst) {
    return addSubExtendedRegGeneric<true, true, false>(core, inst);
}

void subs64Extended(Core* core, AArch64Inst inst) {
    return addSubExtendedRegGeneric<true, true, true>(core, inst);
}

template<bool is64, bool isSub, bool updatesFlags>
void addSubCarryGeneric(Core* core, AArch64Inst inst) {
    uint64_t operand1 = core->GPRegs[inst.Rn];
    uint64_t operand2 = core->GPRegs[inst.Rm];

    if constexpr (isSub) {
        operand2 = ~operand2;
    }

    uint64_t result = addWithCarry<is64, updatesFlags>(operand1, operand2, core->PSTATE.C ? 1 : 0, core->PSTATE);

    if constexpr (!is64) {
        result &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = result;
}

void adc32(Core* core, AArch64Inst inst) {
    return addSubCarryGeneric<false, false, false>(core, inst);
}

void adcs32(Core* core, AArch64Inst inst) {
    return addSubCarryGeneric<false, false, true>(core, inst);
}

void sbc32(Core* core, AArch64Inst inst) {
    return addSubCarryGeneric<false, true, false>(core, inst);
}

void sbcs32(Core* core, AArch64Inst inst) {
    return addSubCarryGeneric<false, true, true>(core, inst);
}

void adc64(Core* core, AArch64Inst inst) {
    return addSubCarryGeneric<true, false, false>(core, inst);
}

void adcs64(Core* core, AArch64Inst inst) {
    return addSubCarryGeneric<true, false, true>(core, inst);
}

void sbc64(Core* core, AArch64Inst inst) {
    return addSubCarryGeneric<true, true, false>(core, inst);
}

void sbcs64(Core* core, AArch64Inst inst) {
    return addSubCarryGeneric<true, true, true>(core, inst);
}

template<bool is64>
void reverseBits(Core* core, AArch64Inst inst) {
    uint64_t op = core->GPRegs[inst.Rn];

    op = ((op <<  1) & 0xAAAAAAAAAAAAAAAA) | ((op >>  1) & 0x5555555555555555);
    op = ((op <<  2) & 0xCCCCCCCCCCCCCCCC) | ((op >>  2) & 0x3333333333333333);
    op = ((op <<  4) & 0xF0F0F0F0F0F0F0F0) | ((op >>  4) & 0x0F0F0F0F0F0F0F0F);
    op = ((op <<  8) & 0xFF00FF00FF00FF00) | ((op >>  8) & 0x00FF00FF00FF00FF);
    op = ((op << 16) & 0xFFFF0000FFFF0000) | ((op >> 16) & 0x0000FFFF0000FFFF);

    if constexpr (!is64) {
        op &= bitmask<32, uint64_t>();
    } else {
        op = ((op << 32) & 0xFFFFFFFF00000000) | ((op >> 32) & 0x00000000FFFFFFFF);
    }

    core->GPRegs[inst.Rd] = op;
}

void rbit32(Core* core, AArch64Inst inst) {
    return reverseBits<false>(core, inst);
}

void rbit64(Core* core, AArch64Inst inst) {
    return reverseBits<true>(core, inst);
}

template<bool is64>
void reverseBytes(Core* core, AArch64Inst inst) {
    uint64_t op = core->GPRegs[inst.Rn];

    op = ((op <<  8) & 0xFF00FF00FF00FF00) | ((op >>  8) & 0x00FF00FF00FF00FF);
    op = ((op << 16) & 0xFFFF0000FFFF0000) | ((op >> 16) & 0x0000FFFF0000FFFF);

    if constexpr (!is64) {
        op &= bitmask<32, uint64_t>();
    } else {
        op = ((op << 32) & 0xFFFFFFFF00000000) | ((op >> 32) & 0x00000000FFFFFFFF);
    }

    core->GPRegs[inst.Rd] = op;
}

void rev32(Core* core, AArch64Inst inst) {
    return reverseBytes<false>(core, inst);
}

void rev64(Core* core, AArch64Inst inst) {
    return reverseBytes<true>(core, inst);
}

template<bool is64>
void reverse16(Core* core, AArch64Inst inst) {
    uint64_t op = core->GPRegs[inst.Rn];

    op = ((op << 16) & 0xFFFF0000FFFF0000) | ((op >> 16) & 0x0000FFFF0000FFFF);

    if constexpr (!is64) {
        op &= bitmask<32, uint64_t>();
    } else {
        op = ((op << 32) & 0xFFFFFFFF00000000) | ((op >> 32) & 0x00000000FFFFFFFF);
    }

    core->GPRegs[inst.Rd] = op;
}

void rev16_32(Core* core, AArch64Inst inst) {
    return reverse16<false>(core, inst);
}

void rev16_64(Core* core, AArch64Inst inst) {
    return reverse16<true>(core, inst);
}

void reverse32(Core* core, AArch64Inst inst) {
    uint64_t op = core->GPRegs[inst.Rn];

    op = ((op << 32) & 0xFFFFFFFF00000000) | ((op >> 32) & 0x00000000FFFFFFFF);

    core->GPRegs[inst.Rd] = op;
}

void rev32_64(Core* core, AArch64Inst inst) {
    return reverse32(core, inst);
}

template<bool is64>
void countLeadingClearBits(Core* core, AArch64Inst inst) {
    // log2 magic?
    uint64_t op = core->GPRegs[inst.Rn];
    int64_t index = (is64)? 63 : 31;

    uint64_t res = 0;

    while ((index >= 0) && !testBit(op, index)) {
        res++;
        index--;
    }

    core->GPRegs[inst.Rd] = res;
}

void clz32(Core* core, AArch64Inst inst) {
    return countLeadingClearBits<false>(core, inst);
}

void clz64(Core* core, AArch64Inst inst) {
    return countLeadingClearBits<true>(core, inst);
}

template<bool is64>
void countLeadingSetBits(Core* core, AArch64Inst inst) {
    // log2 magic?
    uint64_t op = core->GPRegs[inst.Rn];
    int64_t index = (is64)? 63 : 31;

    uint64_t res = 0;

    while ((index >= 0) && testBit(op, index)) {
        res++;
        index--;
    }

    core->GPRegs[inst.Rd] = res;
}

void cls32(Core* core, AArch64Inst inst) {
    return countLeadingSetBits<false>(core, inst);
}

void cls64(Core* core, AArch64Inst inst) {
    return countLeadingSetBits<true>(core, inst);
}

template<bool is64>
void unsignedDivide(Core* core, AArch64Inst inst) {
    uint64_t op1 = core->GPRegs[inst.Rn];
    uint64_t op2 = core->GPRegs[inst.Rm];

    if constexpr (!is64) {
        op1 &= bitmask<32, uint64_t>();
        op2 &= bitmask<32, uint64_t>();
    }

    uint64_t result;
    if (op2 == 0) {
        result = 0;
    } else {
        result = op1 / op2;
    }

    if constexpr (!is64) {
        result &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = result;
}

void udiv32(Core* core, AArch64Inst inst) {
    return unsignedDivide<false>(core, inst);
}

void udiv64(Core* core, AArch64Inst inst) {
    return unsignedDivide<true>(core, inst);
}

template<bool is64>
void signedDivide(Core* core, AArch64Inst inst) {
    uint64_t op1 = core->GPRegs[inst.Rn];
    uint64_t op2 = core->GPRegs[inst.Rm];

    if constexpr (!is64) {
        op1 &= bitmask<32, uint64_t>();
        op1 = signExtend<32>(op1);
        op2 &= bitmask<32, uint64_t>();
        op2 = signExtend<32>(op2);
    }

    uint64_t result;
    if (op2 == 0) {
        result = 0;
    } else if (is64 && (op1 == 0x8000000000000000) && (op2 == 0xFFFFFFFFFFFFFFFF)) {
        // overflow
        result = op1;
    } else {
        // convert to int64_t and do division, then convert back
        int64_t op1_s, op2_s, result_s;
        std::memcpy(&op1_s, &op1, 8);
        std::memcpy(&op2_s, &op2, 8);
        result_s = op1_s / op2_s;
        std::memcpy(&result, &result_s, 8);
    }

    if constexpr (!is64) {
        result &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = result;
}

void sdiv32(Core* core, AArch64Inst inst) {
    return signedDivide<false>(core, inst);
}

void sdiv64(Core* core, AArch64Inst inst) {
    return signedDivide<true>(core, inst);
}

// TODO make shiftType a template?
template<bool is64>
void shiftGeneric(Core* core, AArch64Inst  inst) {
    SHIFT_TYPE shiftType = static_cast<SHIFT_TYPE>(bitRange<10, 11>(inst.bits));

    uint64_t shiftAmount = core->GPRegs[inst.Rm] % ((is64)? 64 : 32);
    uint64_t operand = core->GPRegs[inst.Rn];

    if constexpr (!is64) {
        operand &= bitmask<32, uint64_t>();
    }

    uint64_t res = shift(operand, shiftType, shiftAmount, (is64)? 64 : 32);

    if constexpr (!is64) {
        res &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = res;
}

void lslv32(Core* core, AArch64Inst inst) {
    return shiftGeneric<false>(core, inst);
}

void lsrv32(Core* core, AArch64Inst inst) {
    return shiftGeneric<false>(core, inst);
}

void asrv32(Core* core, AArch64Inst inst) {
    return shiftGeneric<false>(core, inst);
}

void rorv32(Core* core, AArch64Inst inst) {
    return shiftGeneric<false>(core, inst);
}

void lslv64(Core* core, AArch64Inst inst) {
    return shiftGeneric<true>(core, inst);
}

void lsrv64(Core* core, AArch64Inst inst) {
    return shiftGeneric<true>(core, inst);
}

void asrv64(Core* core, AArch64Inst inst) {
    return shiftGeneric<true>(core, inst);
}

void rorv64(Core* core, AArch64Inst inst) {
    return shiftGeneric<true>(core, inst);
}

template<bool is64, bool isSub>
void multiply(Core* core, AArch64Inst inst) {
    uint64_t op1 = core->GPRegs[inst.Rn];
    uint64_t op2 = core->GPRegs[inst.Rm];
    uint64_t op3 = core->GPRegs[inst.Ra];

    uint64_t product = op1 * op2;
    uint64_t res = (isSub)? op3 - product : op3 + product;

    if constexpr (!is64) {
        res &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = res;
}

void madd32(Core* core, AArch64Inst inst) {
    return multiply<false, false>(core, inst);
}

void msub32(Core* core, AArch64Inst inst) {
    return multiply<false, true>(core, inst);
}

void madd64(Core* core, AArch64Inst inst) {
    return multiply<true, false>(core, inst);
}

void msub64(Core* core, AArch64Inst inst) {
    return multiply<true, true>(core, inst);
}

template<bool isSub>
void uMultiplyLong(Core* core, AArch64Inst inst) {
    // 32 x 32 + 64 -> 64
    uint64_t op1 = core->GPRegs[inst.Rn] & bitmask<32, uint64_t>();
    uint64_t op2 = core->GPRegs[inst.Rm] & bitmask<32, uint64_t>();
    uint64_t op3 = core->GPRegs[inst.Ra];

    uint64_t product = op1 * op2;
    uint64_t res = (isSub)? op3 - product : op3 + product;

    core->GPRegs[inst.Rd] = res;
}

void umaddl(Core* core, AArch64Inst inst) {
    return uMultiplyLong<false>(core, inst);
}

void umsubl(Core* core, AArch64Inst inst) {
    return uMultiplyLong<true>(core, inst);
}

template<bool isSub>
void sMultiplyLong(Core* core, AArch64Inst inst) {
    // 32 x 32 + 64 -> 64
    uint64_t op1 = core->GPRegs[inst.Rn] & bitmask<32, uint64_t>();
    uint64_t op2 = core->GPRegs[inst.Rm] & bitmask<32, uint64_t>();
    uint64_t op3 = core->GPRegs[inst.Ra];

    // TODO are these casts right?
    uint32_t op1_32 = static_cast<uint32_t>(op1);
    uint32_t op2_32 = static_cast<uint32_t>(op2);
    int32_t op1_s32 = static_cast<int32_t>(op1_32);
    int32_t op2_s32 = static_cast<int32_t>(op2_32);
    int64_t op1_s64 = static_cast<int64_t>(op1_s32);
    int64_t op2_s64 = static_cast<int64_t>(op2_s32);
    int64_t op3_s64 = static_cast<int64_t>(op3);

    int64_t product = op1_s64 * op2_s64;
    int64_t res = (isSub)? op3_s64 - product : op3_s64 + product;

    // TODO is this cast right?
    core->GPRegs[inst.Rd] = static_cast<uint64_t>(res);
}

void smaddl(Core* core, AArch64Inst inst) {
    return sMultiplyLong<false>(core, inst);
}

void smsubl(Core* core, AArch64Inst inst) {
    return sMultiplyLong<true>(core, inst);
}

template<bool isSigned>
void mulhGeneric(Core* core, AArch64Inst inst) {
    // TODO verify
    uint64_t op1 = core->GPRegs[inst.Rn];
    uint64_t op2 = core->GPRegs[inst.Rm];

    // (a + b) * (c + d)
    uint64_t op1hi = (op1 >> 32); // a
    uint64_t op1lo = (op1 & 0xFFFFFFFF); // b
    uint64_t op2hi = (op2 >> 32); // c
    uint64_t op2lo = (op2 & 0xFFFFFFFF); // d

    uint64_t bd = op1lo * op2lo;
    uint64_t bc = op1lo * op2hi;
    uint64_t ad = op1hi * op2lo;
    uint64_t ac = op1hi * op2hi;

    // get carry into hi quad from bd and low 32bits of ad & bc
    uint32_t cy = static_cast<uint32_t>(((ad & 0xFFFFFFFF) + (bc & 0xFFFFFFFF) + ((bd) >> 32)) >> 32);
    // add in carry, and hi 32bits of ad & bc into hi quad
    uint64_t result = ac + (ad >> 32) + (bc >> 32) + cy;

    // TODO why is this? (got this logic online)
    if constexpr (isSigned) {
        if (testBit<63>(op1)) {
            result -= op2;
        }
        if (testBit<63>(op2)) {
            result -= op1;
        }
    }

    core->GPRegs[inst.Rd] = result;
}

void umulh(Core* core, AArch64Inst inst) {
    return mulhGeneric<false>(core, inst);
}

void smulh(Core* core, AArch64Inst inst) {
    return mulhGeneric<true>(core, inst);
}

template<bool is64, bool negate>
void condCompareRegGeneric(Core* core, AArch64Inst inst) {
    Condition cond = static_cast<Condition>(inst.cond_comp);

    uint64_t operand1 = core->GPRegs[inst.Rn];
    uint64_t operand2 = core->GPRegs[inst.Rm];
    auto& state = core->PSTATE;

    if (testCondition(core, cond)) {
        if constexpr (negate) {
            operand2 = ~operand2;
        }

        addWithCarry<is64, true>(operand1, operand2, negate ? 1 : 0, core->PSTATE);
    } else {
        state.V = testBit<0>(inst.bits);
        state.C = testBit<1>(inst.bits);
        state.Z = testBit<2>(inst.bits);
        state.N = testBit<3>(inst.bits);
    }
}

void ccmp32Reg(Core* core, AArch64Inst inst) {
    condCompareRegGeneric<false, true>(core, inst);
}

void ccmn32Reg(Core* core, AArch64Inst inst) {
    condCompareRegGeneric<false, false>(core, inst);
}

void ccmp64Reg(Core* core, AArch64Inst inst) {
    condCompareRegGeneric<true, true>(core, inst);
}

void ccmn64Reg(Core* core, AArch64Inst inst) {
    condCompareRegGeneric<true, false>(core, inst);
}

template<bool is64, bool negate>
void condCompareImmGeneric(Core* core, AArch64Inst inst) {
    Condition cond = static_cast<Condition>(inst.cond_comp);

    uint64_t operand1 = core->GPRegs[inst.Rn];
    uint64_t operand2 = inst._16_20;
    auto& state = core->PSTATE;

    if (testCondition(core, cond)) {
        if constexpr (negate) {
            operand2 = ~operand2;
        }

        addWithCarry<is64, true>(operand1, operand2, negate ? 1 : 0, core->PSTATE);
    } else {
        state.V = testBit<0>(inst.bits);
        state.C = testBit<1>(inst.bits);
        state.Z = testBit<2>(inst.bits);
        state.N = testBit<3>(inst.bits);
    }
}

void ccmp32Imm(Core* core, AArch64Inst inst) {
    condCompareImmGeneric<false, true>(core, inst);
}

void ccmn32Imm(Core* core, AArch64Inst inst) {
    condCompareImmGeneric<false, false>(core, inst);
}

void ccmp64Imm(Core* core, AArch64Inst inst) {
    condCompareImmGeneric<true, true>(core, inst);
}

void ccmn64Imm(Core* core, AArch64Inst inst) {
    condCompareImmGeneric<true, false>(core, inst);
}

template<bool is64, bool invert, bool increment>
void condSelGeneric(Core* core, AArch64Inst inst) {
    Condition cond = static_cast<Condition>(inst.cond_comp);

    uint64_t op1 = core->GPRegs[inst.Rn];
    uint64_t op2 = core->GPRegs[inst.Rm];
    uint64_t res;

    if (testCondition(core, cond)) {
        res = op1;
    } else {
        res = op2;

        if constexpr (invert) {
            res = ~res;
        }
        if constexpr (increment) {
            res++;
        }
    }

    if constexpr (!is64) {
        res &= bitmask<32, uint64_t>();
    }

    core->GPRegs[inst.Rd] = res;
}

void csel32(Core* core, AArch64Inst inst) {
    condSelGeneric<false, false, false>(core, inst);
}

void csinc32(Core* core, AArch64Inst inst) {
    condSelGeneric<false, false, true>(core, inst);
}

void csinv32(Core* core, AArch64Inst inst) {
    condSelGeneric<false, true, false>(core, inst);
}

void csneg32(Core* core, AArch64Inst inst) {
    condSelGeneric<false, true, true>(core, inst);
}

void csel64(Core* core, AArch64Inst inst) {
    condSelGeneric<true, false, false>(core, inst);
}

void csinc64(Core* core, AArch64Inst inst) {
    condSelGeneric<true, false, true>(core, inst);
}

void csinv64(Core* core, AArch64Inst inst) {
    condSelGeneric<true, true, false>(core, inst);
}

void csneg64(Core* core, AArch64Inst inst) {
    condSelGeneric<true, true, true>(core, inst);
}

void registerDPRegInstructions() {
    registerInstruction(instructionID::UDIV_32, udiv32);
    registerInstruction(instructionID::SDIV_32, sdiv32);
    registerInstruction(instructionID::LSLV_32, lslv32);
    registerInstruction(instructionID::LSRV_32, lsrv32);
    registerInstruction(instructionID::ASRV_32, asrv32);
    registerInstruction(instructionID::RORV_32, rorv32);
    registerInstruction(instructionID::UDIV_64, udiv64);
    registerInstruction(instructionID::SDIV_64, sdiv64);
    registerInstruction(instructionID::LSLV_64, lslv64);
    registerInstruction(instructionID::LSRV_64, lsrv64);
    registerInstruction(instructionID::ASRV_64, asrv64);
    registerInstruction(instructionID::RORV_64, rorv64);
    registerInstruction(instructionID::CRC32B, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::CRC32H, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::CRC32W, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::CRC32X, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::CRC32CB, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::CRC32CH, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::CRC32CW, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::CRC32CX, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::RBIT_32, rbit32);
    registerInstruction(instructionID::REV16_32, rev16_32);
    registerInstruction(instructionID::REV_32, rev32);
    registerInstruction(instructionID::CLZ_32, clz32);
    registerInstruction(instructionID::CLS_32, cls32);
    registerInstruction(instructionID::RBIT_64, rbit64);
    registerInstruction(instructionID::REV16_64, rev16_64);
    registerInstruction(instructionID::REV32_64, rev32_64);
    registerInstruction(instructionID::REV_64, rev64);
    registerInstruction(instructionID::CLZ_64, clz64);
    registerInstruction(instructionID::CLS_64, cls64);
    registerInstruction(instructionID::AND_32_shifted, and32Shifted);
    registerInstruction(instructionID::BIC_32_shifted, bic32Shifted);
    registerInstruction(instructionID::ORR_32_shifted, orr32Shifted);
    registerInstruction(instructionID::ORN_32_shifted, orn32Shifted);
    registerInstruction(instructionID::EOR_32_shifted, eor32Shifted);
    registerInstruction(instructionID::EON_32_shifted, eon32Shifted);
    registerInstruction(instructionID::ANDS_32_shifted, ands32Shifted);
    registerInstruction(instructionID::BICS_32_shifted, bics32Shifted);
    registerInstruction(instructionID::AND_64_shifted, and64Shifted);
    registerInstruction(instructionID::BIC_64_shifted, bic64Shifted);
    registerInstruction(instructionID::ORR_64_shifted, orr64Shifted);
    registerInstruction(instructionID::ORN_64_shifted, orn64Shifted);
    registerInstruction(instructionID::EOR_64_shifted, eor64Shifted);
    registerInstruction(instructionID::EON_64_shifted, eon64Shifted);
    registerInstruction(instructionID::ANDS_64_shifted, ands64Shifted);
    registerInstruction(instructionID::BICS_64_shifted, bics64Shifted);
    registerInstruction(instructionID::ADD_32_shifted, add32Shifted);
    registerInstruction(instructionID::ADDS_32_shifted, adds32Shifted);
    registerInstruction(instructionID::SUB_32_shifted, sub32Shifted);
    registerInstruction(instructionID::SUBS_32_shifted, subs32Shifted);
    registerInstruction(instructionID::ADD_64_shifted, add64Shifted);
    registerInstruction(instructionID::ADDS_64_shifted, adds64Shifted);
    registerInstruction(instructionID::SUB_64_shifted, sub64Shifted);
    registerInstruction(instructionID::SUBS_64_shifted, subs64Shifted);
    registerInstruction(instructionID::ADD_32_extended, add32Extended);
    registerInstruction(instructionID::ADDS_32_extended, adds32Extended);
    registerInstruction(instructionID::SUB_32_extended, sub32Extended);
    registerInstruction(instructionID::SUBS_32_extended, subs32Extended);
    registerInstruction(instructionID::ADD_64_extended, add64Extended);
    registerInstruction(instructionID::ADDS_64_extended, adds64Extended);
    registerInstruction(instructionID::SUB_64_extended, sub64Extended);
    registerInstruction(instructionID::SUBS_64_extended, subs64Extended);
    registerInstruction(instructionID::ADC_32, adc32);
    registerInstruction(instructionID::ADCS_32, adcs32);
    registerInstruction(instructionID::SBC_32, sbc32);
    registerInstruction(instructionID::SBCS_32, sbcs32);
    registerInstruction(instructionID::ADC_64, adc64);
    registerInstruction(instructionID::ADCS_64, adcs64);
    registerInstruction(instructionID::SBC_64, sbc64);
    registerInstruction(instructionID::SBCS_64, sbcs64);
    registerInstruction(instructionID::CCMN_32_reg, ccmn32Reg);
    registerInstruction(instructionID::CCMP_32_reg, ccmp32Reg);
    registerInstruction(instructionID::CCMN_64_reg, ccmn64Reg);
    registerInstruction(instructionID::CCMP_64_reg, ccmp64Reg);
    registerInstruction(instructionID::CCMN_32_imm, ccmn32Imm);
    registerInstruction(instructionID::CCMP_32_imm, ccmp32Imm);
    registerInstruction(instructionID::CCMN_64_imm, ccmn64Imm);
    registerInstruction(instructionID::CCMP_64_imm, ccmp64Imm);
    registerInstruction(instructionID::CSEL_32, csel32);
    registerInstruction(instructionID::CSINC_32, csinc32);
    registerInstruction(instructionID::CSINV_32, csinv32);
    registerInstruction(instructionID::CSNEG_32, csneg32);
    registerInstruction(instructionID::CSEL_64, csel64);
    registerInstruction(instructionID::CSINC_64, csinc64);
    registerInstruction(instructionID::CSINV_64, csinv64);
    registerInstruction(instructionID::CSNEG_64, csneg64);
    registerInstruction(instructionID::MADD_32, madd32);
    registerInstruction(instructionID::MSUB_32, msub32);
    registerInstruction(instructionID::MADD_64, madd64);
    registerInstruction(instructionID::MSUB_64, msub64);
    registerInstruction(instructionID::SMADDL, smaddl);
    registerInstruction(instructionID::SMSUBL, smsubl);
    registerInstruction(instructionID::SMULH, smulh);
    registerInstruction(instructionID::UMADDL, umaddl);
    registerInstruction(instructionID::UMSUBL, umsubl);
    registerInstruction(instructionID::UMULH, umulh);
}

} // namespace interpreter

} // namespace cpu