#include "jit.h"

namespace cpu {

extern SVCHandler gSVCHandler;

namespace jit {

void svc(AArch64EmuAssembler& a, AArch64Inst inst) {
    a.mov(a.argRegs[0], inst._5_20);
    a.envictAll();
    a.call(asmjit::imm_ptr(gSVCHandler));
    //a.int3();
}

void registerSystemInstructions() {
    registerInstruction(instructionID::SVC, svc);
    registerInstruction(instructionID::HVC, FALLBACK);
    registerInstruction(instructionID::SMC, FALLBACK);
    registerInstruction(instructionID::BRK, FALLBACK);
    registerInstruction(instructionID::HLT, FALLBACK);
    registerInstruction(instructionID::DCPS1, FALLBACK);
    registerInstruction(instructionID::DCPS2, FALLBACK);
    registerInstruction(instructionID::DCPS3, FALLBACK);

    registerInstruction(instructionID::MSR_imm, FALLBACK);
    registerInstruction(instructionID::MSR_reg, FALLBACK);
    registerInstruction(instructionID::HINT, nop);
    registerInstruction(instructionID::NOP, nop);
    registerInstruction(instructionID::YIELD, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::WFE, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::WFI, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::SEV, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::SEVL, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::CLREX, FALLBACK);
    registerInstruction(instructionID::DSB, FALLBACK);
    registerInstruction(instructionID::DMB, FALLBACK);
    registerInstruction(instructionID::ISB, nop); // this is for pipelining, which we do not do, so it is a nop (I think)
    registerInstruction(instructionID::SYS, FALLBACK);
    registerInstruction(instructionID::SYSL, FALLBACK);
    registerInstruction(instructionID::MRS, FALLBACK);
}

} // namespace jit

} // namespace cpu