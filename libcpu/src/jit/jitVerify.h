#pragma once
#include "jit\jit.h"
#include "cpu.h"

namespace cpu {

namespace jit {

struct VerifyInfo {
    Core savedState;
};

bool doVerify(instructionID id);

void verifyPreInst(Core* core, VerifyInfo* info, AArch64Inst inst);

void verifyPostInst(Core* core, VerifyInfo* info, AArch64Inst inst);

} // namespace jit

} // namespace cpu
