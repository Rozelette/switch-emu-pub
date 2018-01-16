#pragma once

namespace cpu {

namespace interpreter {

void registerBranchInstructions();
void registerDPImmInstructions();
void registerDPRegInstructions();
void registerFloatInstructions();
void registerLoadStoreInstructions();
void registerSystemInstructions();
void registerSIMDInstructions();

} // namespace interpreter

} // namespace cpu
