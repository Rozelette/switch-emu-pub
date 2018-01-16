#pragma once
#include <stdint.h>
#include <thread>

using arminst_t = uint32_t;
using armaddr_t = uint64_t;

// TODO better format?
union halfFloat {
    uint16_t bits;
    int16_t bitsSigned; // TODO: is this guaranteed to be two's complement?
    struct {
        uint16_t mantissa : 10;
        uint16_t exponent : 5;
        uint16_t sign : 1;
    };

    struct {
        uint16_t : 9;
        uint16_t isQuiet : 1;
        uint16_t : 6;
    };

    static const uint64_t maxExp = 0x1F;
};

union singleFloat {
    uint32_t bits;
    int32_t bitsSigned; // TODO: is this guaranteed to be two's complement?
    float val;
    struct {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
    };

    struct {
        uint32_t : 22;
        uint32_t isQuiet : 1;
        uint32_t : 9;
    };

    static const uint64_t maxExp = 0xFF;
};

union doubleFloat {
    uint64_t bits;
    int64_t bitsSigned; // TODO: is this guaranteed to be two's complement?
    double val;
    struct {
        uint64_t mantissa : 52;
        uint64_t exponent : 11;
        uint64_t sign : 1;
    };

    struct {
        uint64_t : 51;
        uint64_t isQuiet : 1;
        uint64_t : 12;
    };


    static const uint64_t maxExp = 0x7FF;
};

typedef union {
    // TODO do I need a uint128?
    uint64_t D[2];
    uint32_t S[4];
    uint16_t H[8];
    uint8_t B[16];

    halfFloat floatH;
    singleFloat floatS;
    doubleFloat floatD;
} VReg;

struct PSTATE_t {
    bool N; // Negative
    bool Z; // Zero
    bool C; // Carry
    bool V; // Overflow
            // TODO the rest of these
    int SS;
    int IL;
    int nRW;
    int EL;
    int SP;
    int D;
    int A, I, F;
};

// TODO use this
struct ThreadContext {
    uint64_t GPRegs[32]; // 32nd register does not exist, but we will use it for a zero register
    VReg VRegs[32];
    uint64_t PC;
    uint64_t nextPC; // not an actual register
    uint64_t SP;
    PSTATE_t PSTATE;
    // TODO float state
    uint64_t TLSAddr;
};

enum SHIFT_TYPE {
    LSL = 0,
    LSR = 1,
    ASR = 2,
    ROR = 3
};


enum class Condition {
    EQ = 0,
    NE = 1,
    CS = 2,
    CC = 3,
    MI = 4,
    PL = 5,
    VS = 6,
    VC = 7,
    HI = 8,
    LS = 9,
    GE = 10,
    LT = 11,
    GT = 12,
    LE = 13,
    AL = 14,
    A2 = 15
};
