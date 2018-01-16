#include <stdint.h>
#include <numeric>
#include <type_traits>
#include <math.h>

#include "cpu.h"
#include "log.h"
#include "bitutils.h"
#include "instutils.h"
#include "unreachable.h"
#include "floatutils.h"
#include "interpreter.h"

namespace cpu {

namespace interpreter {

// TODO what about the upper bits of VRegs during various instructions?

// TODO better names
enum FloatFlags {
    FloatHalfWidthSource    = 1 << 0,
    FloatSingleWidthSource  = 1 << 1,
    FloatDoubleWidthSource  = 1 << 2,
    FloatHalfWidthDest      = 1 << 3,
    FloatSingleWidthDest    = 1 << 4,
    FloatDoubleWidthDest    = 1 << 5,
    FloatUseZero            = 1 << 6,
    FloatGenerateExceptions = 1 << 7,
    FloatNegate             = 1 << 8,
    FloatCompareQNaNs       = 1 << 9,
    FloatIsSub              = 1 << 10,
    FloatAbsolute           = 1 << 11,
    FloatExact              = 1 << 12,
    FloatUseFPCR            = 1 << 13,
    FloatConditional        = 1 << 14
};

enum RoundingMode {
    RoundEven,
    RoundPosInf,
    RoundNegInf,
    RoundZero,
    RoundAway,
    RoundOdd
};


// helper function for type deduction
template<bool isDouble>
inline auto getReg(Core* core, uint32_t reg) {
    // implement when VS gets constexpr if
    if constexpr (isDouble) {
        return core->VRegs[reg].floatD;
    } else {
        return core->VRegs[reg].floatS;
    }
}

// helper function for type deduction
template<bool isDouble>
inline auto getRegRef(Core* core, uint32_t reg) {
    // implement when VS gets constexpr if
    if constexpr (isDouble) {
        return &core->VRegs[reg].floatD;
    } else {
        return &core->VRegs[reg].floatS;
    }
}

// helper function for type deduction
template<bool isDouble>
inline auto getZero() {
    // implement when VS gets constexpr if
    if constexpr (isDouble) {
        return 0.0;
    } else {
        return 0.0f;
    }
}

// TODO
template<typename T>
T roundFloat(T f, RoundingMode roundingMode) {
    return f;
}

template<>
double roundFloat(double f, RoundingMode roundingMode) {
    switch (roundingMode) {
    case RoundEven:
        // TODO verify
        // nearbyint?
        return f - remainder(f, 1.0);
    case RoundPosInf:
        return ceil(f);
    case RoundNegInf:
        return floor(f);
    case RoundZero:
        return trunc(f);
    case RoundAway:
        return round(f);
    case RoundOdd:
        //UNALLOCATED(core, inst.bits, "RoundOdd in floatRound");
        break;
    default:
        UNREACHABLE();
    }
    return 0.0;
}

template<>
float roundFloat(float f, RoundingMode roundingMode) {
    switch (roundingMode) {
    case RoundEven:
        // TODO verify
        // nearbyint?
        return f - remainderf(f, 1.0f);
    case RoundPosInf:
        return ceilf(f);
    case RoundNegInf:
        return floorf(f);
    case RoundZero:
        return truncf(f);
    case RoundAway:
        return roundf(f);
    case RoundOdd:
        //UNALLOCATED(core, inst.bits, "RoundOdd in floatRound");
        break;
    default:
        UNREACHABLE();
    }
    return 0.0f;
}

void fmovImmToSingle(Core* core, AArch64Inst inst) {
    // data - abcdefgh
    // reg  - aBbbbbbc defgh000 00000000 00000000
    auto& reg = core->VRegs[inst.Rd].floatS;
    uint32_t data = static_cast<uint32_t>(inst._13_20);

    reg.sign = testBit<7>(data)? 1 : 0;
    reg.exponent = (testBit<6>(data)? 0x7C : 0x80) | (bitRange<4, 5>(data));
    reg.mantissa = bitRange<0, 3>(data) << 19;
}

void fmovImmToDouble(Core* core, AArch64Inst inst) {
    // data - abcdefgh
    // reg  - aBbbbbbb bbcdefgh 00000000 00000000 00000000 00000000 00000000 00000000
    auto& reg = core->VRegs[inst.Rd].floatD;
    uint64_t data = static_cast<uint64_t>(inst._13_20);

    reg.sign = testBit<7>(data)? 1 : 0;
    reg.exponent = (testBit<6>(data)? 0x3FC : 0x400) | (bitRange<4, 5>(data));
    reg.mantissa = bitRange<0, 3>(data) << 48;
}

template <unsigned flags>
void floatMoveGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);
    auto reg = getReg<isDouble>(core, inst.Rn);

    if constexpr (flags & FloatNegate) {
        reg.sign = (reg.sign)? 0 : 1;
    } else if constexpr (flags & FloatAbsolute) {
        reg.sign = 0;
    }

    getRegRef<isDouble>(core, inst.Rd)->val = reg.val;
}

void fmovRegToSingle(Core* core, AArch64Inst inst) {
    return floatMoveGeneric<FloatSingleWidthSource>(core, inst);
}

void fabsSingle(Core* core, AArch64Inst inst) {
    return floatMoveGeneric<FloatSingleWidthSource | FloatAbsolute>(core, inst);
}

void fnegSingle(Core* core, AArch64Inst inst) {
    return floatMoveGeneric<FloatSingleWidthSource | FloatNegate>(core, inst);
}

void fmovRegToDouble(Core* core, AArch64Inst inst) {
    return floatMoveGeneric<FloatDoubleWidthSource>(core, inst);
}

void fabsDouble(Core* core, AArch64Inst inst) {
    return floatMoveGeneric<FloatDoubleWidthSource | FloatAbsolute>(core, inst);
}

void fnegDouble(Core* core, AArch64Inst inst) {
    return floatMoveGeneric<FloatDoubleWidthSource | FloatNegate>(core, inst);
}

template<unsigned flags, RoundingMode mode>
void floatRoundGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);
    auto reg = getReg<isDouble>(core, inst.Rn);

    // TODO exact, current mode

    if (isNaN(reg) || isInfinity(reg) || isZero(reg)) {
        // No rounding
        getRegRef<isDouble>(core, inst.Rd)->val = reg.val;
        return;
    }

    // TODO FPCR
    auto roundingMode = (flags & FloatUseFPCR)? RoundNegInf : mode;

    auto old = reg.val;

    reg.val = roundFloat(reg.val, roundingMode);

    /*
    if (isDouble) {
        switch (roundingMode) {
        case cpu::RoundEven:
            // TODO verify
            // nearbyint?
            reg.val -= remainder(reg.val, 1.0);
            break;
        case cpu::RoundPosInf:
            reg.val = ceil(reg.val);
            break;
        case cpu::RoundNegInf:
            reg.val = floor(reg.val);
            break;
        case cpu::RoundZero:
            reg.val = trunc(reg.val);
            break;
        case cpu::RoundAway:
            reg.val = round(reg.val);
            break;
        case cpu::RoundOdd:
            //UNALLOCATED(core, inst.bits, "RoundOdd in floatRound");
            break;
        default:
            UNREACHABLE();
        }
    } else {
        switch (mode) {
        case cpu::RoundEven:
            // TODO verify
            reg.val -= remainderf(reg.val, 1.0f);
            break;
        case cpu::RoundPosInf:
            reg.val = ceilf(reg.val);
            break;
        case cpu::RoundNegInf:
            reg.val = floorf(reg.val);
            break;
        case cpu::RoundZero:
            reg.val = truncf(reg.val);
            break;
        case cpu::RoundAway:
            reg.val = roundf(reg.val);
            break;
        case cpu::RoundOdd:
            //UNALLOCATED(core, inst.bits, "RoundOdd in floatRound");
            break;
        default:
            UNREACHABLE();
        }
    }
    */

    if constexpr (flags & FloatExact) {
        if (old != reg.val) {
            // TODO Inexact exception
            logger::warn("TODO: Floating-point round raised Inexact exception");
        }
    }

    getRegRef<isDouble>(core, inst.Rd)->val = reg.val;
}

void frintnSingle(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatSingleWidthSource, RoundEven>(core, inst);
}

void frintpSingle(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatSingleWidthSource, RoundPosInf>(core, inst);
}

void frintmSingle(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatSingleWidthSource, RoundNegInf>(core, inst);
}

void frintzSingle(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatSingleWidthSource, RoundZero>(core, inst);
}

void frintaSingle(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatSingleWidthSource, RoundAway>(core, inst);
}

void frintxSingle(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatSingleWidthSource | FloatExact | FloatUseFPCR, RoundNegInf>(core, inst);
}

void frintiSingle(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatSingleWidthSource | FloatUseFPCR, RoundNegInf>(core, inst);
}

void frintnDouble(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatDoubleWidthSource, RoundEven>(core, inst);
}

void frintpDouble(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatDoubleWidthSource, RoundPosInf>(core, inst);
}

void frintmDouble(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatDoubleWidthSource, RoundNegInf>(core, inst);
}

void frintzDouble(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatDoubleWidthSource, RoundZero>(core, inst);
}

void frintaDouble(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatDoubleWidthSource, RoundAway>(core, inst);
}

void frintxDouble(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatDoubleWidthSource | FloatExact | FloatUseFPCR, RoundNegInf>(core, inst);
}

void frintiDouble(Core* core, AArch64Inst inst) {
    return floatRoundGeneric<FloatDoubleWidthSource | FloatUseFPCR, RoundNegInf>(core, inst);
}

template <unsigned flags>
void floatSqrtGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);
    auto reg = getReg<isDouble>(core, inst.Rn);

    if (isNaN(reg)) {
        // do nothing
    } else if (isZero(reg)) {
        // do nothing
    } else if (isPositiveInfinity(reg)) {
        // do nothing
    } else if (isNegative(reg)) {
        // TODO Invalid Operation exception
        logger::warn("TODO: Floating-point sqrt raised Invalid Operation exception");
        // TODO default Nan
        reg.val = generateQNaN<decltype(reg.val)>();
    } else {
        // TODO actual inplementation
        reg.val = sqrt(reg.val);
    }

    getRegRef<isDouble>(core, inst.Rd)->val = reg.val;
}

void fsqrtSingle(Core* core, AArch64Inst inst) {
    return floatSqrtGeneric<FloatSingleWidthSource>(core, inst);
}

void fsqrtDouble(Core* core, AArch64Inst inst) {
    return floatSqrtGeneric<FloatDoubleWidthSource>(core, inst);
}

template <unsigned flags>
void floatConvertGeneric(Core* core, AArch64Inst inst) {
    uint64_t sign;
    uint64_t exponent;
    uint64_t mantissa;

    // TODO actually do lol
    // TODO rounding, inf, nan, zero, etc...

    if constexpr (flags & FloatHalfWidthSource) {
        auto reg = core->VRegs[inst.Rn].floatH;
        sign = reg.sign;
        exponent = static_cast<uint64_t>(reg.exponent) - 15;
        mantissa = static_cast<uint64_t>(reg.mantissa) << 42;
    } else if constexpr (flags & FloatSingleWidthSource) {
        auto reg = core->VRegs[inst.Rn].floatS;
        sign = reg.sign;
        exponent = static_cast<uint64_t>(reg.exponent) - 127;
        mantissa = static_cast<uint64_t>(reg.mantissa) << 29;
    } else if constexpr (flags & FloatDoubleWidthSource) {
        auto reg = core->VRegs[inst.Rn].floatD;
        sign = reg.sign;
        exponent = reg.exponent- 1023;
        mantissa = reg.mantissa;
    }

    if constexpr (flags & FloatHalfWidthDest) {
        auto& reg = core->VRegs[inst.Rd].floatH;
        reg.sign = sign;
        reg.exponent = exponent + 15;
        reg.mantissa = mantissa >> 42;
    } else if constexpr (flags & FloatSingleWidthDest) {
        auto& reg = core->VRegs[inst.Rd].floatS;
        reg.sign = sign;
        reg.exponent = exponent + 127;
        reg.mantissa = mantissa >> 29;
    } else if constexpr (flags & FloatDoubleWidthDest) {
        auto& reg = core->VRegs[inst.Rd].floatD;
        reg.sign = sign;
        reg.exponent = exponent + 1023;
        reg.mantissa = mantissa;
    }
}

void fcvtSingleToDouble(Core* core, AArch64Inst inst) {
    return floatConvertGeneric<FloatSingleWidthSource | FloatDoubleWidthDest>(core, inst);
}

void fcvtSingleToHalf(Core* core, AArch64Inst inst) {
    return floatConvertGeneric<FloatSingleWidthSource | FloatHalfWidthDest>(core, inst);
}

void fcvtDoubleToSingle(Core* core, AArch64Inst inst) {
    return floatConvertGeneric<FloatDoubleWidthSource | FloatSingleWidthDest>(core, inst);
}

void fcvtDoubleToHalf(Core* core, AArch64Inst inst) {
    return floatConvertGeneric<FloatDoubleWidthSource | FloatHalfWidthDest>(core, inst);
}

void fcvtHalfToSingle(Core* core, AArch64Inst inst) {
    return floatConvertGeneric<FloatHalfWidthSource | FloatSingleWidthDest>(core, inst);
}

void fcvtHalfToDouble(Core* core, AArch64Inst inst) {
    return floatConvertGeneric<FloatDoubleWidthSource | FloatDoubleWidthDest>(core, inst);
}

template<unsigned flags>
void floatMulGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);

    auto reg1 = getReg<isDouble>(core, inst.Rn);
    auto reg2 = getReg<isDouble>(core, inst.Rm);
    // TODO Nans

    auto result = getZero<isDouble>();

    if ((isInfinity(reg1) && isZero(reg2)) || (isZero(reg1) && isInfinity(reg2))) {
        // TODO Invalid Operation exception
        logger::warn("TODO: Floating-point multiply raised Invalid Operation exception");
        // TODO default Nan
        result = generateQNaN<decltype(result)>();
    } else if (isInfinity(reg1) || isInfinity(reg2)) {
        // TODO infinity function
        result = std::numeric_limits<decltype(result)>::infinity();
        if (reg1.sign != reg2.sign) {
            result = -result;
        }
    } else if (isZero(reg1) || isZero(reg2)) {
        // TODO does this work?
        result = (reg1.sign == reg2.sign)? getZero<isDouble>() : -getZero<isDouble>();
    } else {
        result = reg1.val * reg2.val;
        // TODO rounding
    }

    if constexpr (flags & FloatNegate) {
        result = -result;
    }

    getRegRef<isDouble>(core, inst.Rd)->val = result;
}

void fmulSingle(Core* core, AArch64Inst inst) {
    return floatMulGeneric<FloatSingleWidthSource>(core, inst);
}

void fnmulSingle(Core* core, AArch64Inst inst) {
    return floatMulGeneric<FloatSingleWidthSource | FloatNegate>(core, inst);
}

void fmulDouble(Core* core, AArch64Inst inst) {
    return floatMulGeneric<FloatDoubleWidthSource>(core, inst);
}

void fnmulDouble(Core* core, AArch64Inst inst) {
    return floatMulGeneric<FloatDoubleWidthSource | FloatNegate>(core, inst);
}

template<unsigned flags>
void floatDivGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);

    auto reg1 = getReg<isDouble>(core, inst.Rn);
    auto reg2 = getReg<isDouble>(core, inst.Rm);
    // TODO Nans

    auto result = getZero<isDouble>();

    if ((isInfinity(reg1) && isInfinity(reg2)) || (isZero(reg1) && isZero(reg2))) {
        // TODO Invalid Operation exception
        logger::warn("TODO: Floating-point divide raised Invalid Operation exception");
        // TODO default Nan
        result = generateQNaN<decltype(result)>();
    } else if(isZero(reg2)) {
        result = (reg1.sign == reg2.sign)? getZero<isDouble>() : -getZero<isDouble>();
        // TODO Divide by Zero exception
        logger::warn("TODO: Floating-point divide raised Divide by Zero exception");
    } else if (isInfinity(reg1)) {
        // TODO infinity function
        result = std::numeric_limits<decltype(result)>::infinity();
        if (reg1.sign != reg2.sign) {
            result = -result;
        }
    } else if (isZero(reg1) || isInfinity(reg2)) {
        result = (reg1.sign == reg2.sign)? getZero<isDouble>() : -getZero<isDouble>();
    } else {
        result = reg1.val / reg2.val;
        // TODO rounding
    }

    getRegRef<isDouble>(core, inst.Rd)->val = result;
}

void fdivSingle(Core* core, AArch64Inst inst) {
    return floatDivGeneric<FloatSingleWidthSource>(core, inst);
}

void fdivDouble(Core* core, AArch64Inst inst) {
    return floatDivGeneric<FloatDoubleWidthSource>(core, inst);
}

template<unsigned flags>
void floatMaxGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);

    auto reg1 = getReg<isDouble>(core, inst.Rn);
    auto reg2 = getReg<isDouble>(core, inst.Rm);
    // TODO Nans

    if constexpr (flags & FloatCompareQNaNs) {
        if (isQuietNaN(reg1) && !isQuietNaN(reg2)) {
            // TODO infinity function
            reg1.val = -std::numeric_limits<decltype(reg1.val)>::infinity();
        } else if (!isQuietNaN(reg1) && isQuietNaN(reg2)) {
            // TODO infinity function
            reg2.val = -std::numeric_limits<decltype(reg2.val)>::infinity();
        }
    }

    auto result = (reg1.val > reg2.val)? reg1.val : reg2.val;

    if ((result == 0.0) && (isPositive(reg1) || isPositive(reg2))) {
        // result may be -0.0
        result = 0.0;
    }

    getRegRef<isDouble>(core, inst.Rd)->val = result;
}

void fmaxSingle(Core* core, AArch64Inst inst) {
    return floatMaxGeneric<FloatSingleWidthSource>(core, inst);
}

void fmaxnmSingle(Core* core, AArch64Inst inst) {
    return floatMaxGeneric<FloatSingleWidthSource | FloatCompareQNaNs>(core, inst);
}

void fmaxDouble(Core* core, AArch64Inst inst) {
    return floatMaxGeneric<FloatDoubleWidthSource>(core, inst);
}

void fmaxnmDouble(Core* core, AArch64Inst inst) {
    return floatMaxGeneric<FloatDoubleWidthSource | FloatCompareQNaNs>(core, inst);
}

template<unsigned flags>
void floatMinGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);

    auto reg1 = getReg<isDouble>(core, inst.Rn);
    auto reg2 = getReg<isDouble>(core, inst.Rm);
    // TODO Nans

    if constexpr (flags & FloatCompareQNaNs) {
        if (isQuietNaN(reg1) && !isQuietNaN(reg2)) {
            // TODO infinity function
            reg1.val = std::numeric_limits<decltype(reg1.val)>::infinity();
        } else if (!isQuietNaN(reg1) && isQuietNaN(reg2)) {
            // TODO infinity function
            reg2.val = std::numeric_limits<decltype(reg2.val)>::infinity();
        }
    }

    auto result = (reg1.val < reg2.val)? reg1.val : reg2.val;

    if ((result == 0.0) && (isNegative(reg1) || isNegative(reg2))) {
        // result may be 0.0
        result = -0.0;
    }

    getRegRef<isDouble>(core, inst.Rd)->val = result;
}

void fminSingle(Core* core, AArch64Inst inst) {
    return floatMinGeneric<FloatSingleWidthSource>(core, inst);
}

void fminnmSingle(Core* core, AArch64Inst inst) {
    return floatMinGeneric<FloatSingleWidthSource | FloatCompareQNaNs>(core, inst);
}

void fminDouble(Core* core, AArch64Inst inst) {
    return floatMinGeneric<FloatDoubleWidthSource>(core, inst);
}

void fminnmDouble(Core* core, AArch64Inst inst) {
    return floatMinGeneric<FloatDoubleWidthSource | FloatCompareQNaNs>(core, inst);
}

template<unsigned flags>
void floatAddGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);

    auto reg1 = getReg<isDouble>(core, inst.Rn);
    auto reg2 = getReg<isDouble>(core, inst.Rm);
    // TODO Nans

    auto result = getZero<isDouble>();

    if (isInfinity(reg1) && isInfinity(reg2) && (reg1.sign != reg2.sign)) {
        // TODO Invalid Operation exception
        logger::warn("TODO: Floating-point add raised Invalid Operation exception");
        // TODO default Nan
        result = generateQNaN<decltype(result)>();
    } else if (isInfinity(reg1) && isInfinity(reg2)) {
        // TODO infinity function
        result = reg1.val;
    } else if (isZero(reg1) && isZero(reg2) && (reg1.sign == reg2.sign)) {
        result = reg1.val;
    } else {
        result = reg1.val + reg2.val;
        // TODO rounding
    }

    getRegRef<isDouble>(core, inst.Rd)->val = result;
}

void faddSingle(Core* core, AArch64Inst inst) {
    return floatAddGeneric<FloatSingleWidthSource>(core, inst);
}

void faddDouble(Core* core, AArch64Inst inst) {
    return floatAddGeneric<FloatDoubleWidthSource>(core, inst);
}

template<unsigned flags>
void floatSubGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);

    auto reg1 = getReg<isDouble>(core, inst.Rn);
    auto reg2 = getReg<isDouble>(core, inst.Rm);
    // TODO Nans

    auto result = getZero<isDouble>();

    if (isInfinity(reg1) && isInfinity(reg2) && (reg1.sign == reg2.sign)) {
        // TODO Invalid Operation exception
        logger::warn("TODO: Floating-point sub raised Invalid Operation exception");
        // TODO default Nan
        result = generateQNaN<decltype(result)>();
    } else if (isPositiveInfinity(reg1) && isNegativeInfinity(reg2)) {
        // TODO infinity function
        result = reg1.val;
    } else if (isNegativeInfinity(reg1) && isPositiveInfinity(reg2)) {
        // TODO infinity function
        result = reg1.val;
    } else if (isZero(reg1) && isZero(reg2) && (reg1.sign != reg2.sign)) {
        result = reg1.val;
    } else {
        result = reg1.val - reg2.val;
        // TODO rounding
    }

    getRegRef<isDouble>(core, inst.Rd)->val = result;
}

void fsubSingle(Core* core, AArch64Inst inst) {
    return floatSubGeneric<FloatSingleWidthSource>(core, inst);
}

void fsubDouble(Core* core, AArch64Inst inst) {
    return floatSubGeneric<FloatDoubleWidthSource>(core, inst);
}

template<unsigned flags>
void floatMulAddGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);

    auto reg1 = getReg<isDouble>(core, inst.Rn);
    auto reg2 = getReg<isDouble>(core, inst.Rm);
    auto reg3 = getReg<isDouble>(core, inst.Ra);
    // TODO Nans

    auto result = getZero<isDouble>();

    if constexpr (flags & FloatIsSub) {
        reg1.sign = reg1.sign? 0 : 1;
    }

    auto inf1 = isInfinity(reg1);
    auto zero1 = isZero(reg1);
    auto sign1 = isNegative(reg1);
    auto inf2 = isInfinity(reg2);
    auto zero2 = isZero(reg2);
    auto sign2 = isNegative(reg2);
    auto inf3 = isInfinity(reg3);
    auto zero3 = isZero(reg3);
    auto sign3 = isNegative(reg3);

    auto infp = inf1 || inf2;
    auto zerop = zero1 || zero2;
    auto signp = sign1 != sign2;

    if (isQuietNaN(reg3) && ((inf1 && zero2) || (zero1 && inf2))) {
        // TODO Invalid Operation exception
        logger::warn("TODO: Floating-point fused Multiply-Add raised Invalid Operation exception");
        // TODO default Nan
        result = generateQNaN<decltype(result)>();
    }

    // TODO other condition
    if ((inf1 && zero2) || (zero1 && inf2) || (inf3 && infp && (sign3 != signp))) {
        // TODO Invalid Operation exception
        logger::warn("TODO: Floating-point fused Multiply-Add raised Invalid Operation exception");
        // TODO default Nan
        result = generateQNaN<decltype(result)>();
    } else if ((inf3 && !sign3) || (infp && !signp)) {
        // TODO infinity function
        reg1.val = std::numeric_limits<decltype(reg1.val)>::infinity();
    } else if ((inf3 && sign3) || (infp && signp)) {
        // TODO infinity function
        reg1.val = -std::numeric_limits<decltype(reg1.val)>::infinity();
    } else if (zero3 && zerop && (sign3 == signp)) {
        result = (reg3.sign)? -getZero<isDouble>() : getZero<isDouble>();
    } else {
        result = reg3.val + (reg1.val * reg2.val);
        // TODO rounding
    }

    if constexpr (flags & FloatNegate) {
        result = -result;
    }

    getRegRef<isDouble>(core, inst.Rd)->val = result;
}

void fmaddSingle(Core* core, AArch64Inst inst) {
    return floatMulAddGeneric<FloatSingleWidthSource>(core, inst);
}

void fmsubSingle(Core* core, AArch64Inst inst) {
    return floatMulAddGeneric<FloatSingleWidthSource | FloatIsSub>(core, inst);
}

void fnmaddSingle(Core* core, AArch64Inst inst) {
    return floatMulAddGeneric<FloatSingleWidthSource | FloatNegate>(core, inst);
}

void fnmsubSingle(Core* core, AArch64Inst inst) {
    return floatMulAddGeneric<FloatSingleWidthSource | FloatNegate | FloatIsSub>(core, inst);
}

void fmaddDouble(Core* core, AArch64Inst inst) {
    return floatMulAddGeneric<FloatDoubleWidthSource>(core, inst);
}

void fmsubDouble(Core* core, AArch64Inst inst) {
    return floatMulAddGeneric<FloatDoubleWidthSource | FloatIsSub>(core, inst);
}

void fnmaddDouble(Core* core, AArch64Inst inst) {
    return floatMulAddGeneric<FloatDoubleWidthSource | FloatNegate>(core, inst);
}

void fnmsubDouble(Core* core, AArch64Inst inst) {
    return floatMulAddGeneric<FloatDoubleWidthSource | FloatNegate | FloatIsSub>(core, inst);
}

template<unsigned flags>
void floatCondSelectGeneric(Core* core, AArch64Inst inst) {
    Condition cond = static_cast<Condition>(inst.cond_comp);

    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);

    getRegRef<isDouble>(core, inst.Rd)->val = (testCondition(core, cond))? getReg<isDouble>(core, inst.Rn).val : getReg<isDouble>(core, inst.Rm).val;
}

void fcselSingle(Core* core, AArch64Inst inst) {
    return floatCondSelectGeneric<FloatSingleWidthSource>(core, inst);
}

void fcselDouble(Core* core, AArch64Inst inst) {
    return floatCondSelectGeneric<FloatDoubleWidthSource>(core, inst);
}

template<unsigned flags>
void floatCompareGeneric(Core* core, AArch64Inst inst) {
    Condition cond = static_cast<Condition>(inst.cond_comp);

    constexpr bool isDouble = !!(flags & FloatDoubleWidthSource);

    auto reg1 = getReg<isDouble>(core, inst.Rn);
    auto reg2 = getReg<isDouble>(core, inst.Rm);

    // TODO maybe just have a zero reg?
    if constexpr (flags & FloatUseZero) {
        reg2.bits = 0;
    }

    if constexpr (flags & FloatConditional) {
        if (!testCondition(core, cond)) {
            core->PSTATE.N = testBit<3>(inst.bits);
            core->PSTATE.Z = testBit<2>(inst.bits);
            core->PSTATE.C = testBit<1>(inst.bits);
            core->PSTATE.V = testBit<0>(inst.bits);
            return;
        }
    }

    bool raiseException = (flags & FloatGenerateExceptions)? isNaN(reg1) || isNaN(reg2) : isSignalingNaN(reg1) || isSignalingNaN(reg2);
    if (raiseException) {
        // TODO Invalid Operation exception
        logger::warn("TODO: Floating-point compare raised Invalid Operation exception");
        core->PSTATE.N = false;
        core->PSTATE.Z = false;
        core->PSTATE.C = true;
        core->PSTATE.V = true;
        return;
    }

    if (reg1.bitsSigned == reg2.bitsSigned) {
        core->PSTATE.N = false;
        core->PSTATE.Z = true;
        core->PSTATE.C = true;
        core->PSTATE.V = false;
    } else if (reg1.bitsSigned < reg2.bitsSigned) {
        core->PSTATE.N = true;
        core->PSTATE.Z = false;
        core->PSTATE.C = false;
        core->PSTATE.V = false;
    } else if (reg1.bitsSigned > reg2.bitsSigned) {
        core->PSTATE.N = false;
        core->PSTATE.Z = false;
        core->PSTATE.C = true;
        core->PSTATE.V = false;
    }
}

void fcmpSingle(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatSingleWidthSource>(core, inst);
}

void fcmpZeroSingle(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatSingleWidthSource | FloatUseZero>(core, inst);
}

void fcmpeSingle(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatSingleWidthSource | FloatGenerateExceptions>(core, inst);
}

void fcmpeZeroSingle(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatSingleWidthSource | FloatUseZero | FloatGenerateExceptions>(core, inst);
}

void fccmpSingle(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatSingleWidthSource | FloatConditional>(core, inst);
}

void fccmpeSingle(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatSingleWidthSource | FloatGenerateExceptions | FloatConditional>(core, inst);
}

void fcmpDouble(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatDoubleWidthSource>(core, inst);
}

void fcmpZeroDouble(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatDoubleWidthSource | FloatUseZero>(core, inst);
}

void fcmpeDouble(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatDoubleWidthSource | FloatGenerateExceptions>(core, inst);
}

void fcmpeZeroDouble(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatDoubleWidthSource | FloatUseZero | FloatGenerateExceptions>(core, inst);
}

void fccmpDouble(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatDoubleWidthSource | FloatConditional>(core, inst);
}

void fccmpeDouble(Core* core, AArch64Inst inst) {
    return floatCompareGeneric<FloatDoubleWidthSource | FloatGenerateExceptions | FloatConditional>(core, inst);
}

enum ConvertFlags {
    ConvertNoFlags = 0,
    Convert64Bit   = 1 << 0,
    ConvertDouble  = 1 << 1,
    ConvertSigned  = 1 << 2
};

template <unsigned flags>
void floatConvIntToFloatGeneric(Core* core, AArch64Inst inst) {
    auto val = core->GPRegs[inst.Rn];

    constexpr bool is64 = !!(flags & Convert64Bit);
    constexpr bool isDouble = !!(flags & ConvertDouble);

    if (!is64) {
        val &= bitmask<32, uint64_t>();
    }

    auto reg = getRegRef<isDouble>(core, inst.Rd);

    uint64_t exponent = 0;
    uint64_t mantissa = 0;

    if constexpr (flags & ConvertSigned) {
        if (testBit<is64 ? 63 : 31>(val)) {
            reg->sign = 1;
            val = (~val) + 1; // 2's compliment
        }
    }

    if (val != 0) {
        if (val >= (1llu << ((isDouble)? 53 : 24))) {
            uint64_t bias = 0;
            while (val >= (1llu << ((isDouble)? 53 : 24))) {
                if (testBit<0>(val)) {
                    //logger::warn("TODO: inexact in floatConvIntToFloatGeneric");
                    // TODO inexact
                }
                val >>= 1;
                bias++;
            }

            exponent = ((isDouble)? 1023 : 127) + ((isDouble)? 52 : 23) + bias;
            mantissa = val;
        } else {
            // adjust so leading 1 is the implicit 1 in the mantissa
            uint64_t bias = 0;
            while (!testBit<(isDouble)? 52 : 23>(val)) {
                val <<= 1;
                bias++;
            }

            exponent = ((isDouble)? 1023 : 127) + ((isDouble)? 52 : 23) - bias;
            mantissa = val;
        }
    }

    reg->exponent = exponent;
    reg->mantissa = mantissa; // note: this will drop the implicit 1
}

void ucvtf32ToSingle(Core* core, AArch64Inst inst) {
    return floatConvIntToFloatGeneric<ConvertNoFlags>(core, inst);
}

void scvtf32ToSingle(Core* core, AArch64Inst inst) {
    return floatConvIntToFloatGeneric<ConvertSigned>(core, inst);
}

void ucvtf32ToDouble(Core* core, AArch64Inst inst) {
    return floatConvIntToFloatGeneric<ConvertDouble>(core, inst);
}

void scvtf32ToDouble(Core* core, AArch64Inst inst) {
    return floatConvIntToFloatGeneric<ConvertDouble | ConvertSigned>(core, inst);
}

void ucvtf64ToSingle(Core* core, AArch64Inst inst) {
    return floatConvIntToFloatGeneric<Convert64Bit>(core, inst);
}

void scvtf64ToSingle(Core* core, AArch64Inst inst) {
    return floatConvIntToFloatGeneric<Convert64Bit | ConvertSigned>(core, inst);
}

void ucvtf64ToDouble(Core* core, AArch64Inst inst) {
    return floatConvIntToFloatGeneric<ConvertDouble | Convert64Bit>(core, inst);
}

void scvtf64ToDouble(Core* core, AArch64Inst inst) {
    return floatConvIntToFloatGeneric<ConvertDouble | Convert64Bit | ConvertSigned>(core, inst);
}

enum IntMovFlags {
    IntMovNoFlags = 0,
    IntMovFloat   = 1 << 0,
    IntMovDouble  = 1 << 1,
    IntMovUpper   = 1 << 2,
    IntMovToInt   = 1 << 3
};

template <unsigned flags>
void floatIntMovGeneric(Core* core, AArch64Inst inst) {
    if constexpr (flags & IntMovToInt) {
        if constexpr (flags & IntMovFloat) {
            core->GPRegs[inst.Rd] = core->VRegs[inst.Rn].floatS.bits;
        } else if constexpr (flags & IntMovDouble) {
            core->GPRegs[inst.Rd] = core->VRegs[inst.Rn].floatD.bits;
        } else if constexpr (flags & IntMovUpper) {
            core->GPRegs[inst.Rd] = core->VRegs[inst.Rn].D[1];
        } else {
            UNREACHABLE();
        }
    } else {
        if constexpr (flags & IntMovFloat) {
            core->VRegs[inst.Rd].floatS.bits = static_cast<uint32_t>(core->GPRegs[inst.Rn]);
        } else if constexpr (flags & IntMovDouble) {
            core->VRegs[inst.Rd].floatD.bits = core->GPRegs[inst.Rn];
        } else if constexpr (flags & IntMovUpper) {
            core->VRegs[inst.Rd].D[1] = core->GPRegs[inst.Rn];
        } else {
            UNREACHABLE();
        }
    }
}

void fmovSingleTo32(Core* core, AArch64Inst inst) {
    return floatIntMovGeneric<IntMovFloat | IntMovToInt>(core, inst);
}

void fmov32ToSingle(Core* core, AArch64Inst inst) {
    return floatIntMovGeneric<IntMovFloat>(core, inst);
}

void fmovDoubleto64(Core* core, AArch64Inst inst) {
    return floatIntMovGeneric<IntMovDouble | IntMovToInt>(core, inst);
}

void fmov64ToDouble(Core* core, AArch64Inst inst) {
    return floatIntMovGeneric<IntMovDouble>(core, inst);
}

void fmovUpperTo64(Core* core, AArch64Inst inst) {
    return floatIntMovGeneric<IntMovUpper | IntMovToInt>(core, inst);
}

void fmov64ToUpper(Core* core, AArch64Inst inst) {
    return floatIntMovGeneric<IntMovUpper>(core, inst);
}

template <unsigned flags, RoundingMode mode>
void floatConvertToIntGeneric(Core* core, AArch64Inst inst) {
    constexpr bool isDouble = !!(flags & ConvertDouble);

    auto reg = getReg<isDouble>(core, inst.Rn);

    uint64_t data = 0;
    double temp = 0.0;
    switch (mode) {
    case RoundEven:
        // TODO verify
        // nearbyint?
        temp = reg.val - remainder(reg.val, 1.0);
        break;
    case RoundPosInf:
        temp = ceil(reg.val);
        break;
    case RoundNegInf:
        temp = floor(reg.val);
        break;
    case RoundZero:
        temp = trunc(reg.val);
        break;
    case RoundAway:
        temp = round(reg.val);
        break;
    case RoundOdd: // not implemented in aarch64
    default:
        UNREACHABLE();
        break;
    }

    // TODO exceptions
    if constexpr (flags & ConvertSigned) {
        if constexpr (flags & Convert64Bit) {
            data = static_cast<int64_t>(temp);
        } else {
            data = static_cast<int32_t>(temp);
        }
    } else {
        if constexpr (flags & Convert64Bit) {
            data = static_cast<uint64_t>(temp);
        } else {
            data = static_cast<uint32_t>(temp);
        }
    }

    core->GPRegs[inst.Rd] = data;
}

void fcvtnsSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertSigned, RoundEven>(core, inst);
}

void fcvtnuSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertNoFlags, RoundEven>(core, inst);
}

void fcvtasSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertSigned, RoundAway>(core, inst);
}

void fcvtauSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertNoFlags, RoundAway>(core, inst);
}

void fcvtpsSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertSigned, RoundPosInf>(core, inst);
}

void fcvtpuSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertNoFlags, RoundPosInf>(core, inst);
}

void fcvtmsSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertSigned, RoundNegInf>(core, inst);
}

void fcvtmuSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertNoFlags, RoundNegInf>(core, inst);
}

void fcvtzsSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertSigned, RoundZero>(core, inst);
}

void fcvtzuSingleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertNoFlags, RoundZero>(core, inst);
}

void fcvtnsDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble | ConvertSigned, RoundEven>(core, inst);
}

void fcvtnuDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble, RoundEven>(core, inst);
}

void fcvtasDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble | ConvertSigned, RoundAway>(core, inst);
}

void fcvtauDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble, RoundAway>(core, inst);
}

void fcvtpsDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble | ConvertSigned, RoundPosInf>(core, inst);
}

void fcvtpuDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble, RoundPosInf>(core, inst);
}

void fcvtmsDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble | ConvertSigned, RoundNegInf>(core, inst);
}

void fcvtmuDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble, RoundNegInf>(core, inst);
}

void fcvtzsDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble | ConvertSigned, RoundZero>(core, inst);
}

void fcvtzuDoubleTo32Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<ConvertDouble, RoundZero>(core, inst);
}

void fcvtnsSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertSigned, RoundEven>(core, inst);
}

void fcvtnuSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit, RoundEven>(core, inst);
}

void fcvtasSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertSigned, RoundAway>(core, inst);
}

void fcvtauSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit, RoundAway>(core, inst);
}

void fcvtpsSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertSigned, RoundPosInf>(core, inst);
}

void fcvtpuSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit, RoundPosInf>(core, inst);
}

void fcvtmsSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertSigned, RoundNegInf>(core, inst);
}

void fcvtmuSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit, RoundNegInf>(core, inst);
}

void fcvtzsSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertSigned, RoundZero>(core, inst);
}

void fcvtzuSingleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit, RoundZero>(core, inst);
}

void fcvtnsDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble | ConvertSigned, RoundEven>(core, inst);
}

void fcvtnuDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble, RoundEven>(core, inst);
}

void fcvtasDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble | ConvertSigned, RoundAway>(core, inst);
}

void fcvtauDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble, RoundAway>(core, inst);
}

void fcvtpsDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble | ConvertSigned, RoundPosInf>(core, inst);
}

void fcvtpuDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble, RoundPosInf>(core, inst);
}

void fcvtmsDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble | ConvertSigned, RoundNegInf>(core, inst);
}

void fcvtmuDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble, RoundNegInf>(core, inst);
}

void fcvtzsDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble | ConvertSigned, RoundZero>(core, inst);
}

void fcvtzuDoubleTo64Int(Core* core, AArch64Inst inst) {
    return floatConvertToIntGeneric<Convert64Bit | ConvertDouble, RoundZero>(core, inst);
}

void registerFloatInstructions() {
    registerInstruction(instructionID::SCVTF_32_single_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::UCVTF_32_single_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::FCVTZS_single_32_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::FCVTZU_single_32_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::SCVTF_32_double_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::UCVTF_32_double_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::FCVTZS_double_32_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::FCVTZU_double_32_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::SCVTF_64_single_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::UCVTF_64_single_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::FCVTZS_single_64_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::FCVTZU_single_64_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::SCVTF_64_double_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::UCVTF_64_double_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::FCVTZS_double_64_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::FCVTZU_double_64_fixed, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::FCVTNS_single_32_int, fcvtnsSingleTo32Int);
    registerInstruction(instructionID::FCVTNU_single_32_int, fcvtnuSingleTo32Int);
    registerInstruction(instructionID::SCVTF_32_single_int, scvtf32ToSingle);
    registerInstruction(instructionID::UCVTF_32_single_int, ucvtf32ToSingle);
    registerInstruction(instructionID::FCVTAS_single_32_int, fcvtasSingleTo32Int);
    registerInstruction(instructionID::FCVTAU_single_32_int, fcvtauSingleTo32Int);
    registerInstruction(instructionID::FMOV_single_32, fmovSingleTo32);
    registerInstruction(instructionID::FMOV_32_single, fmov32ToSingle);
    registerInstruction(instructionID::FCVTPS_single_32_int, fcvtpsSingleTo32Int);
    registerInstruction(instructionID::FCVTPU_single_32_int, fcvtpuSingleTo32Int);
    registerInstruction(instructionID::FCVTMS_single_32_int, fcvtmsSingleTo32Int);
    registerInstruction(instructionID::FCVTMU_single_32_int, fcvtmuSingleTo32Int);
    registerInstruction(instructionID::FCVTZS_single_32_int, fcvtzsSingleTo32Int);
    registerInstruction(instructionID::FCVTZU_single_32_int, fcvtzuSingleTo32Int);
    registerInstruction(instructionID::FCVTNS_double_32_int, fcvtnsDoubleTo32Int);
    registerInstruction(instructionID::FCVTNU_double_32_int, fcvtnuDoubleTo32Int);
    registerInstruction(instructionID::SCVTF_32_double_int, scvtf32ToDouble);
    registerInstruction(instructionID::UCVTF_32_double_int, ucvtf32ToDouble);
    registerInstruction(instructionID::FCVTAS_double_32_int, fcvtasDoubleTo32Int);
    registerInstruction(instructionID::FCVTAU_double_32_int, fcvtauDoubleTo32Int);
    registerInstruction(instructionID::FCVTPS_double_32_int, fcvtpsDoubleTo32Int);
    registerInstruction(instructionID::FCVTPU_double_32_int, fcvtpuDoubleTo32Int);
    registerInstruction(instructionID::FCVTMS_double_32_int, fcvtmsDoubleTo32Int);
    registerInstruction(instructionID::FCVTMU_double_32_int, fcvtmuDoubleTo32Int);
    registerInstruction(instructionID::FCVTZS_double_32_int, fcvtzsDoubleTo32Int);
    registerInstruction(instructionID::FCVTZU_double_32_int, fcvtzuDoubleTo32Int);
    registerInstruction(instructionID::FCVTNS_single_64_int, fcvtnsSingleTo64Int);
    registerInstruction(instructionID::FCVTNU_single_64_int, fcvtnuSingleTo64Int);
    registerInstruction(instructionID::SCVTF_64_single_int, scvtf64ToSingle);
    registerInstruction(instructionID::UCVTF_64_single_int, ucvtf64ToSingle);
    registerInstruction(instructionID::FCVTAS_single_64_int, fcvtasSingleTo64Int);
    registerInstruction(instructionID::FCVTAU_single_64_int, fcvtauSingleTo64Int);
    registerInstruction(instructionID::FCVTPS_single_64_int, fcvtpsSingleTo64Int);
    registerInstruction(instructionID::FCVTPU_single_64_int, fcvtpuSingleTo64Int);
    registerInstruction(instructionID::FCVTMS_single_64_int, fcvtmsSingleTo64Int);
    registerInstruction(instructionID::FCVTMU_single_64_int, fcvtmuSingleTo64Int);
    registerInstruction(instructionID::FCVTZS_single_64_int, fcvtzsSingleTo64Int);
    registerInstruction(instructionID::FCVTZU_single_64_int, fcvtzuSingleTo64Int);
    registerInstruction(instructionID::FCVTNS_double_64_int, fcvtnsDoubleTo64Int);
    registerInstruction(instructionID::FCVTNU_double_64_int, fcvtnuDoubleTo64Int);
    registerInstruction(instructionID::SCVTF_64_double_int, scvtf64ToDouble);
    registerInstruction(instructionID::UCVTF_64_double_int, ucvtf64ToDouble);
    registerInstruction(instructionID::FCVTAS_double_64_int, fcvtasDoubleTo64Int);
    registerInstruction(instructionID::FCVTAU_double_64_int, fcvtauDoubleTo64Int);
    registerInstruction(instructionID::FMOV_double_64, fmovDoubleto64);
    registerInstruction(instructionID::FMOV_64_double, fmov64ToDouble);
    registerInstruction(instructionID::FCVTPS_double_64_int, fcvtpsDoubleTo64Int);
    registerInstruction(instructionID::FCVTPU_double_64_int, fcvtpuDoubleTo64Int);
    registerInstruction(instructionID::FCVTMS_double_64_int, fcvtmsDoubleTo64Int);
    registerInstruction(instructionID::FCVTMU_double_64_int, fcvtmuDoubleTo64Int);
    registerInstruction(instructionID::FCVTZS_double_64_int, fcvtzsDoubleTo64Int);
    registerInstruction(instructionID::FCVTZU_double_64_int, fcvtzuDoubleTo64Int);
    registerInstruction(instructionID::FMOV_top128_64, fmovUpperTo64);
    registerInstruction(instructionID::FMOV_64_top128, fmov64ToUpper);
    registerInstruction(instructionID::FMOV_reg_single, fmovRegToSingle);
    registerInstruction(instructionID::FABS_single, fabsSingle);
    registerInstruction(instructionID::FNEG_single, fnegSingle);
    registerInstruction(instructionID::FSQRT_single, fsqrtSingle);
    registerInstruction(instructionID::FCVT_single_double, fcvtSingleToDouble);
    registerInstruction(instructionID::FCVT_single_half, fcvtSingleToHalf);
    registerInstruction(instructionID::FRINTN_single, frintnSingle);
    registerInstruction(instructionID::FRINTP_single, frintpSingle);
    registerInstruction(instructionID::FRINTM_single, frintmSingle);
    registerInstruction(instructionID::FRINTZ_single, frintzSingle);
    registerInstruction(instructionID::FRINTA_single, frintaSingle);
    registerInstruction(instructionID::FRINTX_single, frintxSingle);
    registerInstruction(instructionID::FRINTI_single, frintiSingle);
    registerInstruction(instructionID::FMOV_reg_double, fmovRegToDouble);
    registerInstruction(instructionID::FABS_double, fabsDouble);
    registerInstruction(instructionID::FNEG_double, fnegDouble);
    registerInstruction(instructionID::FSQRT_double, fsqrtDouble);
    registerInstruction(instructionID::FCVT_double_single, fcvtDoubleToSingle);
    registerInstruction(instructionID::FCVT_double_half, fcvtDoubleToHalf);
    registerInstruction(instructionID::FRINTN_double, frintnDouble);
    registerInstruction(instructionID::FRINTP_double, frintpDouble);
    registerInstruction(instructionID::FRINTM_double, frintmDouble);
    registerInstruction(instructionID::FRINTZ_double, frintzDouble);
    registerInstruction(instructionID::FRINTA_double, frintaDouble);
    registerInstruction(instructionID::FRINTX_double, frintxDouble);
    registerInstruction(instructionID::FRINTI_double, frintiDouble);
    registerInstruction(instructionID::FCVT_half_single, fcvtHalfToSingle);
    registerInstruction(instructionID::FCVT_half_double, fcvtHalfToDouble);
    registerInstruction(instructionID::FCMP_single, fcmpSingle);
    registerInstruction(instructionID::FCMP_single_zero, fcmpZeroSingle);
    registerInstruction(instructionID::FCMPE_single, fcmpeSingle);
    registerInstruction(instructionID::FCMPE_single_zero, fcmpeZeroSingle);
    registerInstruction(instructionID::FCMP_double, fcmpDouble);
    registerInstruction(instructionID::FCMP_double_zero, fcmpZeroDouble);
    registerInstruction(instructionID::FCMPE_double, fcmpeDouble);
    registerInstruction(instructionID::FCMPE_double_zero, fcmpeZeroDouble);
    registerInstruction(instructionID::FMOV_imm_single, fmovImmToSingle);
    registerInstruction(instructionID::FMOV_imm_double, fmovImmToDouble);
    registerInstruction(instructionID::FCCMP_single, fccmpSingle);
    registerInstruction(instructionID::FCCMPE_single, fccmpeSingle);
    registerInstruction(instructionID::FCCMP_double, fccmpDouble);
    registerInstruction(instructionID::FCCMPE_double, fccmpeDouble);
    registerInstruction(instructionID::FMUL_single, fmulSingle);
    registerInstruction(instructionID::FDIV_single, fdivSingle);
    registerInstruction(instructionID::FADD_single, faddSingle);
    registerInstruction(instructionID::FSUB_single, fsubSingle);
    registerInstruction(instructionID::FMAX_single, fmaxSingle);
    registerInstruction(instructionID::FMIN_single, fminSingle);
    registerInstruction(instructionID::FMAXNM_single, fmaxnmSingle);
    registerInstruction(instructionID::FMINNM_single, fminnmSingle);
    registerInstruction(instructionID::FNMUL_single, fnmulSingle);
    registerInstruction(instructionID::FMUL_double, fmulDouble);
    registerInstruction(instructionID::FDIV_double, fdivDouble);
    registerInstruction(instructionID::FADD_double, faddDouble);
    registerInstruction(instructionID::FSUB_double, fsubDouble);
    registerInstruction(instructionID::FMAX_double, fmaxDouble);
    registerInstruction(instructionID::FMIN_double, fminDouble);
    registerInstruction(instructionID::FMAXNM_double, fmaxnmDouble);
    registerInstruction(instructionID::FMINNM_double, fminnmDouble);
    registerInstruction(instructionID::FNMUL_double, fnmulDouble);
    registerInstruction(instructionID::FCSEL_single, fcselSingle);
    registerInstruction(instructionID::FCSEL_double, fcselDouble);
    registerInstruction(instructionID::FMADD_single, fmaddSingle);
    registerInstruction(instructionID::FMSUB_single, fmsubSingle);
    registerInstruction(instructionID::FNMADD_single, fnmaddSingle);
    registerInstruction(instructionID::FNMSUB_single, fnmsubSingle);
    registerInstruction(instructionID::FMADD_double, fmaddDouble);
    registerInstruction(instructionID::FMSUB_double, fmsubDouble);
    registerInstruction(instructionID::FNMADD_double, fnmaddDouble);
    registerInstruction(instructionID::FNMSUB_double, fnmsubDouble);
}

} // namespace interpreter

} // namespace cpu