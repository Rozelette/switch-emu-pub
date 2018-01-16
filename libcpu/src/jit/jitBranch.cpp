#include <map>

#include "jit.h"
#include "bitutils.h"

namespace cpu {

namespace jit {

extern std::map<uint64_t, JitCode> gBlockMap;

void insertEndOfBlock(AArch64EmuAssembler& a, uint64_t addr) {
    write64ImmToMem(a, a.coreNextPCMem, addr);
    a.envictAll();

    auto findBlock = gBlockMap.find(addr);
    if (findBlock != gBlockMap.end()) {
        // block has already been jitted, so let's jump to it
        a.jmp(asmjit::imm_ptr((*findBlock).second));
    } else {
        // block has not been generated, so let's just end this one
        // TODO when it is generated, we should rewrite the end of this block to jump to it instead
        a.jmp(asmjit::imm_ptr(gBlockFinale));
        logger::info("Jit could not find block at addr: %016llX", addr);
    }
}

void condBranch(AArch64EmuAssembler& a, AArch64Inst inst) {
    Condition cond = static_cast<Condition>(inst.cond);
    uint64_t off = signExtend<21>(static_cast<uint64_t>(inst._5_23 << 2));

    auto endLabel = a.newLabel();

    auto testReg = a.allocGpTemp(1);
    a.insertTestCondition(cond, testReg);
    a.test(testReg, testReg);

    // Envict registers here because they need to be envicted regardless of execution path
    a.envictAll();

    a.jpe(endLabel);

    insertEndOfBlock(a, a.currentPC + off);

    a.bind(endLabel);

    write64ImmToMem(a, a.coreNextPCMem, a.currentPC + 4);
}

template <bool link>
void uncondBranchRegGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    if (link) {
        a.mov(a.loadGpRegisterWrite(a.coreGPRegs[30]), a.currentPC + 4);
    }

    a.mov(a.coreNextPCMem, a.loadGpRegisterRead(a.coreGPRegs[inst.Rn]));

    a.envictAll();
    a.jmp(asmjit::imm_ptr(gBlockFinale));
}

void uncondBranchRegNoLink(AArch64EmuAssembler& a, AArch64Inst inst) {
    return uncondBranchRegGeneric<false>(a, inst);
}

void uncondBranchRegLink(AArch64EmuAssembler& a, AArch64Inst inst) {
    return uncondBranchRegGeneric<true>(a, inst);
}

template <bool link>
void uncondBranchImmGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    uint64_t off = signExtend<28>(static_cast<uint64_t>(inst._0_25 << 2));

    if (link) {
        auto linkReg = a.loadGpRegisterWrite(a.coreGPRegs[30]);
        a.mov(linkReg, a.currentPC + 4);
    }

    insertEndOfBlock(a, a.currentPC + off);
}

void uncondBranchImmNoLink(AArch64EmuAssembler& a, AArch64Inst inst) {
    return uncondBranchImmGeneric<false>(a, inst);
}

void uncondBranchImmLink(AArch64EmuAssembler& a, AArch64Inst inst) {
    return uncondBranchImmGeneric<true>(a, inst);
}

void registerBranchInstructions() {
    registerInstruction(instructionID::B_cond, condBranch);
    registerInstruction(instructionID::BR, uncondBranchRegNoLink);
    registerInstruction(instructionID::BLR, uncondBranchRegLink);
    registerInstruction(instructionID::RET, uncondBranchRegNoLink);
    registerInstruction(instructionID::ERET, FALLBACK);
    registerInstruction(instructionID::DRPS, FALLBACK);
    registerInstruction(instructionID::B, uncondBranchImmNoLink);
    registerInstruction(instructionID::BL, uncondBranchImmLink);
    registerInstruction(instructionID::CBZ_32, FALLBACK);
    registerInstruction(instructionID::CBNZ_32, FALLBACK);
    registerInstruction(instructionID::CBZ_64, FALLBACK);
    registerInstruction(instructionID::CBNZ_64, FALLBACK);
    registerInstruction(instructionID::TBZ, FALLBACK);
    registerInstruction(instructionID::TBNZ, FALLBACK);
}

} // namespace jit

} // namespace cpu