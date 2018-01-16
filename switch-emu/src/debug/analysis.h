#pragma once
#include <string>
#include "instructionTable.h"
#include "instruction.h"
#include "cpu.h"
#include "decode.h"

namespace debug::analysis {

struct FuncInfo {
    std::string name;
};

struct AddrInfo {
    FuncInfo* func;
};

struct BranchInfo {
    uint64_t target;
    bool isVariable;
    // TODO isConditional, isConditionSatisfied, isCall, isRet
};

void init();

AddrInfo get(uint64_t addr);
BranchInfo getBranchInfo(cpu::DecodedInstruction& inst, uint64_t addr, cpu::Core* state); // TODO move into cpu::meta

} // namespace debug::analysis
