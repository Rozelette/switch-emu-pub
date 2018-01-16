#pragma once
#include <string>
#include <vector>

#include "instruction.h"
#include "decode.h"

namespace cpu {

struct Disassembly {
    std::string name;
    std::string args;
};

namespace disassembler {

enum class RegType {
    X, W, B, H, S, D, Q
};

enum FieldType {
    NONE,
    cond_branch,
    cond_comp,
    Wt, Xt, Bt, Ht, St, Dt, Qt,
    Wt2, Xt2, Bt2, Ht2, St2, Dt2, Qt2,
    Wn, Xn, Bn, Hn, Sn, Dn, Qn,
    Wn_WSP, Xn_SP,
    Wm, Xm, Bm, Hm, Sm, Dm, Qm,
    Wa, Xa, Ba, Ha, Sa, Da, Qa,
    Wd, Xd, Bd, Hd, Sd, Dd, Qd,
    Wd_WSP, Xd_SP,
    Ws,
    imm_tbz,
    imm_exceptions,
    imm_hint,
    imm_clrex,
    imm_adr,
    imm_adrp,
    imm_addsub,
    imm_logical,
    imm_mov,
    imm_bitfield,
    imm_extract,
    imm_compare,
    imm_shift,
    label_cond_branch,
    label_branch,
    label_tbz,
    label_loadstore,
    prefetch_op,
    barrier_type,
    shifted_reg,
    W_extended_reg,
    X_extended_reg,
    cond_comp_flags,
    loadstore_no_off,
    loadstore_post_off,
    loadstore_pre_off,
    loadstore_unscaled_off,
    loadstore_unsigned_off_8,
    loadstore_unsigned_off_16,
    loadstore_unsigned_off_32,
    loadstore_unsigned_off_64,
    loadstore_unsigned_off_128,
    loadstore_post_off_pair_32,
    loadstore_post_off_pair_64,
    loadstore_post_off_pair_128,
    loadstore_pre_off_pair_32,
    loadstore_pre_off_pair_64,
    loadstore_pre_off_pair_128,
    loadstore_off_pair_32,
    loadstore_off_pair_64,
    loadstore_off_pair_128,
    loadstore_reg,
    float_zero_imm,
    TODO,
};

struct InstructionInfo {
    instructionID id;
    std::string name;
    std::vector<FieldType> regs;
    uint32_t mask;
    uint32_t condition;
    std::string rawName;
};

bool init();

std::string getBaseInstructionName(cpu::instructionID id);
bool disassemble(DecodedInstruction& dInst, Disassembly& dis, uint64_t addr);

} // namespace disassembler

} // namespace cpu
