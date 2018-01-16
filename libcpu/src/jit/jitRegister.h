#pragma once

namespace cpu {

namespace jit {

void registerBranchInstructions();
void registerSystemInstructions();
void registerDPImmInstructions();
void registerDPRegInstructions();
void registerFloatInstructions();
void registerLoadStoreInstructions();

} // namespace jit

} // namespace cpu
