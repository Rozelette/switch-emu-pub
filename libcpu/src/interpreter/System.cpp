#include <stdio.h>
#include <atomic>

#include "cpu.h"
#include "instutils.h"
#include "interpreter.h"

namespace cpu {

extern SVCHandler gSVCHandler;

namespace interpreter {

// TODO should holf timing information in Core state
uint64_t gTimer = 0;

void svc(Core* core, AArch64Inst inst) {
    gSVCHandler(inst._5_20);
}

// TODO unified MRS/MSR logic
void mrs(Core* core, AArch64Inst inst) {
    if (inst.o0 == 1 && inst.op1 == 3 && inst.CRn == 13 && inst.CRm == 0 && inst.op2 == 2) {
        // TPIDR_EL0
        core->GPRegs[inst.Rd] = core->TLSAddr; // how is this different than TPIDRRO_EL0?
        logger::warn("TPIDR_EL0 read");
    } else if (inst.o0 == 1 && inst.op1 == 3 && inst.CRn == 13 && inst.CRm == 0 && inst.op2 == 3) {
        // TPIDRRO_EL0
        core->GPRegs[inst.Rd] = core->TLSAddr; // how is this different than TPIDR_EL0?
    } else if (inst.o0 == 1 && inst.op1 == 3 && inst.CRn == 14 && inst.CRm == 0 && inst.op2 == 1) {
        // CNTPCT_EL0
        core->GPRegs[inst.Rd] = gTimer++; // TODO actually get some sensible timings
    } else if (inst.o0 == 1 && inst.op1 == 3 && inst.CRn == 4 && inst.CRm == 4 && inst.op2 == 0) {
        // FPCR
        logger::warn("Unhandled MRS FPCR");
        core->GPRegs[inst.Rd] = 0;
    } else if (inst.o0 == 1 && inst.op1 == 3 && inst.CRn == 4 && inst.CRm == 4 && inst.op2 == 1) {
        // FPSR
        logger::warn("Unhandled MRS FPSR");
        core->GPRegs[inst.Rd] = 0;
    } else if (inst.o0 == 1 && inst.op1 == 3 && inst.CRn == 0 && inst.CRm == 0 && inst.op2 == 7) {
        // DCZID_EL0
        // TODO support DC ZVA?
        core->GPRegs[inst.Rd] = 0x10;
    } else {
        logger::error("Unhandled MRS system reg o0:%d op1:%d CRn:%1X CRm:%1X op2:%d", inst.o0, inst.op1, inst.CRn, inst.CRm, inst.op2);
        core->GPRegs[inst.Rd] = 0;
    }
}

void msrImm(Core* core, AArch64Inst inst) {
    logger::error("Unhandled MSR immediate");
}

void msrReg(Core* core, AArch64Inst inst) {
    if (inst.o0 == 1 && inst.op1 == 3 && inst.CRn == 4 && inst.CRm == 4 && inst.op2 == 0) {
        // FPCR
        logger::warn("Unhandled MSR FPCR");
    } else if (inst.o0 == 1 && inst.op1 == 3 && inst.CRn == 4 && inst.CRm == 4 && inst.op2 == 1) {
        // FPSR
        logger::warn("Unhandled MSR FPSR");
    } else if (inst.o0 == 1 && inst.op1 == 3 && inst.CRn == 13 && inst.CRm == 0 && inst.op2 == 2) {
        // TPIDR_ELO
        logger::warn("Unhandled MSR TPIDR_ELO");
    } else {
        logger::error("Unhandled MSR system reg o0:%d op1:%d CRn:%1X CRm:%1X op2:%d", inst.o0, inst.op1, inst.CRn, inst.CRm, inst.op2);
    }
}

void clrex(Core* core, AArch64Inst inst) {
    core->exclusiveAccess = false;
}

void dmb(Core* core, AArch64Inst inst) {
    // TODO what exactly is the proper way to do this?
    std::_Atomic_thread_fence(std::memory_order_seq_cst);
}

void sys(Core* core, AArch64Inst inst) {
    // TODO

    uint64_t op1 = inst.op1;
    uint64_t crn = inst.CRn;
    uint64_t crm = inst.CRm;
    uint64_t op2 = inst.op2;
    uint64_t data = core->GPRegs[inst.Rt];

    if ((op1 == 0x3) && (crn == 0x7) && (crm == 0x5) && (op2 == 0x1)) {
        // IC IVAU
        // TODO: "When SCTLR_EL1.UCI == 1, accessible from EL0 or higher. Otherwise, accessible from EL1 or higher."
        // Since this is called from the sdk (in EL0), does that imply SCTLR_EL1.UCI == 1?

        // Invalidate instruction cache by address to Point of Unification
        // NOP since we have no instruction cache
        // TODO Permission Fault if EL0 does not have read access for VA
    } else if ((op1 == 0x3) && (crn == 0x7) && (crm == 0xB) && (op2 == 0x1)) {
        // DC CVAU
        // TODO: "When SCTLR_EL1.UCI == 1, accessible from EL0 or higher. Otherwise, accessible from EL1 or higher."
        // Since this is called from the sdk (in EL0), does that imply SCTLR_EL1.UCI == 1?
        
        // Clean data cache by address to Point of Unification
        // NOP since we have no data cache
        // TODO Permission Fault if EL0 does not have read access for VA
    } else if ((op1 == 0x3) && (crn == 0x7) && (crm == 0xE) && (op2 == 0x1)) {
        // DC CIVAC
        // TODO: "When SCTLR_EL1.UCI == 1, accessible from EL0 or higher. Otherwise, accessible from EL1 or higher."
        // Since this is called from the sdk (in EL0), does that imply SCTLR_EL1.UCI == 1?

        // Clean and Invalidate data cache by address to Point of Coherency
        // NOP since we have no data cache
        // TODO Permission Fault if EL0 does not have read access for VA
    } else {
        logger::error("Unhandled SYS with op1: %02X, CRn: %02X, CRm: %02X, op2: %02X, data: %016llX", op1, crn, crm, op2, data);
    }
}

void registerSystemInstructions() {
    registerInstruction(instructionID::SVC, svc);
    registerInstruction(instructionID::HVC, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::SMC, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::BRK, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::HLT, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::DCPS1, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::DCPS2, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::DCPS3, UNIMPLEMENTED_FUNC);

    registerInstruction(instructionID::MSR_imm, msrImm);
    registerInstruction(instructionID::MSR_reg, msrReg);
    registerInstruction(instructionID::HINT, nop);
    registerInstruction(instructionID::NOP, nop);
    registerInstruction(instructionID::YIELD, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::WFE, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::WFI, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::SEV, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::SEVL, nop); // TODO can we do something smarter?
    registerInstruction(instructionID::CLREX, clrex);
    registerInstruction(instructionID::DSB, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::DMB, dmb);
    registerInstruction(instructionID::ISB, nop); // this is for pipelining, which we do not do, so it is a nop
    registerInstruction(instructionID::SYS, sys);
    registerInstruction(instructionID::SYSL, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::MRS, mrs);
}

} // namespace interpreter

} // namespace cpu