#pragma once
#include "types.h"
#include "instruction.h"
#include "cpu.h"

#include "bitutils.h"
#include "unreachable.h"

inline bool testCondition(cpu::Core* core, Condition cond) {
    auto state = core->PSTATE;
    bool condMet = false;

    switch (cond) {
    case Condition::EQ:
        condMet = state.Z;
        break;
    case Condition::NE:
        condMet = !state.Z;
        break;
    case Condition::CS:
        condMet = state.C;
        break;
    case Condition::CC:
        condMet = !state.C;
        break;
    case Condition::MI:
        condMet = state.N;
        break;
    case Condition::PL:
        condMet = !state.N;
        break;
    case Condition::VS:
        condMet = state.V;
        break;
    case Condition::VC:
        condMet = !state.V;
        break;
    case Condition::HI:
        condMet = state.C && !state.Z;
        break;
    case Condition::LS:
        condMet = !(state.C && !state.Z);
        break;
    case Condition::GE:
        condMet = (state.N == state.V);
        break;
    case Condition::LT:
        condMet = !(state.N == state.V);
        break;
    case Condition::GT:
        condMet = (state.N == state.V) && !state.Z;
        break;
    case Condition::LE:
        condMet = !((state.N == state.V) && !state.Z);
        break;
    case Condition::AL:
    case Condition::A2:
        condMet = true;
        break;
    default:
        UNREACHABLE();
    }

    return condMet;
}

inline uint64_t shift(uint64_t val, SHIFT_TYPE t, uint64_t amount, uint64_t bits) {
    if (amount == 0) {
        return val;
    }
    switch (t) {
    case LSL:
        return val << amount;
    case LSR:
        return val >> amount;
    case ASR:
    {
        // Note: right shift on signed values is IMPLEMENTATION DEFINED so we can not rely on a certain behavior
        // TODO does this work?
        uint64_t res = val >> amount;
        res = signExtend(res, bits - amount);

        return res;
    }
    case ROR:
        return (val >> amount) | (val << (bits - amount));
    default:
        UNREACHABLE();
        return val;
    }
}

inline uint64_t getLogicalImmediate(const cpu::AArch64Inst& inst) {
    uint64_t size = (static_cast<uint64_t>(inst.N) << 6) | ((~static_cast<uint64_t>(inst.imms)) & bitmask<6, uint64_t>());
    uint64_t length = 0;
    for (; size > 1; length++, size >>= 1);
    // TODO assert for length?
    uint64_t levels = bitmask(length);
    uint64_t S = inst.imms & levels;
    uint64_t R = inst.immr & levels;
    uint64_t esize = 1llu << length;
    uint64_t welem = bitmask(S + 1);
    welem = (welem >> R) | ((welem << (esize - R)) & bitmask(esize)); // ROR(welem, R)
    uint64_t ret = 0;
    for (uint64_t i = 0u; i < 64 / esize; i++) {
        ret |= welem;
        welem <<= esize;
    }
    return ret;
}

template<bool is64, bool updateFlags>
inline uint64_t addWithCarry(uint64_t op1, uint64_t op2, uint64_t carry, PSTATE_t& state) {
    uint64_t result = op1 + op2 + carry;

    if constexpr (!is64) {
        result &= bitmask<32, uint64_t>();
    }

    if (updateFlags) {
        bool op1MSB = testBit<(is64) ? 63 : 31>(op1);
        bool op2MSB = testBit<(is64) ? 63 : 31>(op2);
        bool resMSB = testBit<(is64) ? 63 : 31>(result);
        state.N = resMSB;
        state.Z = (result == 0);
        state.V = (op1MSB == op2MSB) && (op1MSB != resMSB);

        uint64_t mask = (is64) ? bitmask<63, uint64_t>() : bitmask<31, uint64_t>();
        uint64_t partial = (op1 & mask) + (op2 & mask) + carry;
        bool parMSB = testBit<(is64) ? 63 : 31>(partial);
        state.C = (op1MSB && op2MSB) || (op1MSB && parMSB) || (op2MSB && parMSB);
    }

    return result;
}
