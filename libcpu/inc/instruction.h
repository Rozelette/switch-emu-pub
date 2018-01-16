#pragma once
#include <stdint.h>

#include "instructionTable.h"

namespace cpu {

// TODO get rid of struct
struct AArch64Inst {
    union {
        uint32_t bits;

#define FLD(name, from, to) \
        struct { \
            uint32_t : (from); \
            uint32_t name : ((to) - (from) + 1); \
            uint32_t : (31 - (to)); \
        };

        FLD(cond, 0, 3); // condition for branch
        FLD(cond_comp, 12, 15); // condition for conditional compare
        FLD(_0_25, 0, 25); // immediate offset for unconditional branch
        FLD(_5_11, 5, 11);
        FLD(_5_18, 5, 18);
        FLD(_5_20, 5, 20); // immediate value for SVC and immediate moves
        FLD(_5_23, 5, 23); // immediate offset for conditional branch and load stores
        FLD(_8_11, 8, 11);
        FLD(_10_12, 10, 12); // immediate value for some DPReg operations
        FLD(_10_15, 10, 15); // immediate value for some DPReg operations
        FLD(_10_21, 10, 21); // immediate value for some DPImm operations
        FLD(_12_20, 12, 20);
        FLD(_13_20, 13, 20); // immediate value for some float operations
        FLD(_15_21, 15, 21); // immediate value for some Load/Store pair operations
        FLD(_16_20, 16, 20); // immediate value for some DPReg operations
        FLD(_22_23, 22, 23); // size value for some SIMD operations
        // TODO combine system registers?
        FLD(o0, 19, 19); // System register?
        FLD(op1, 16, 18); // System register?
        FLD(CRn, 12, 15); // System register?
        FLD(CRm, 8, 11); // System register? Also used for barriers
        FLD(op2, 5, 7); // System register?
        FLD(option, 13, 15); // extend option for extened register instructions
        FLD(hw, 21, 22); // shift amount for mov instructions
        FLD(opc, 29, 30); // additional opcode for instructions (TODO get rid of this?)
        FLD(shift, 22, 23); // shift type
        FLD(Rt, 0, 4); // register to test against/store
        FLD(Rd, 0, 4); // destination register for data processing operations
        FLD(Rn, 5, 9); // input register 1
        FLD(Rm, 16, 20); // input register 2
        FLD(Ra, 10, 14); // input register 3
        FLD(Rt2, 10, 14); // 2nd register to store
        FLD(Rs, 16, 20); // status register for load/store exclusive
        FLD(immlo, 29, 30); // low portion of immediate for PC-rel. addressing
        FLD(immhi, 5, 23); // high portion of immediate for PC-rel. addressing
        FLD(imms, 10, 15); // part of immediate for DPImm operations
        FLD(immr, 16, 21); // part of immediate for DPImm operations
        FLD(N, 22, 22); // part of immediate for DPImm operations
        FLD(b40, 19, 23); // immediate for TB(N)Z
        FLD(b5, 31, 31); // immediate for TB(N)Z
        FLD(nzcv, 0, 3); // flags set for cond compare
#undef FLD
    };
};

} // namespace cpu