#include "jit.h"
#include "bitutils.h"
#include "mem.h"

namespace cpu {

namespace jit {

// It is implementation defined what happens with writeback when Rt == Rn and Rn != 31.
// For the jit, we have decided to treat is as a nop

enum StoreFlags {
    StoreWriteBack = 1 << 0,
    StoreImmSigned = 1 << 1,
    StorePostIndex = 1 << 2,
    StoreFloat = 1 << 3,
    Store128BitFloat = 1 << 4,
    StoreRelease = 1 << 5
};

template <typename T, unsigned flags = 0>
void storeRegImmGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    if (((flags & StoreWriteBack) != 0) && (inst.Rt == inst.Rn) && (inst.Rn != 31)) return;

    constexpr uint64_t numBits = constLog2(sizeof(T));

    uint64_t imm;
    if constexpr (flags & StoreImmSigned) {
        imm = static_cast<uint64_t>(bitRange<12, 20>(inst.bits));
        imm = signExtend<9>(imm);
    } else {
        imm = static_cast<uint64_t>(bitRange<10, 21>(inst.bits));
        imm <<= numBits;
    }

    if (inst.Rn == 31) {
        // TODO check SP alignment
        a.mov(a.argRegs[0], a.loadGpRegisterRead(a.coreSPMem));
    } else {
        a.mov(a.argRegs[0], a.loadGpRegisterRead(a.coreGPRegs[inst.Rn]));
    }

    if constexpr (!(flags & StorePostIndex)) {
        a.add(a.argRegs[0], imm);
    }

    if constexpr (flags & StoreFloat) {
        a.mov(a.argRegs[1], a.loadGpRegisterRead(a.coreVRegs[inst.Rt][0]));
    } else {
        a.mov(a.argRegs[1], a.loadGpRegisterRead(a.coreGPRegs[inst.Rt]));
    }
    // TODO is this needed?
    a.and_(a.argRegs[1], bitmask<(1 << numBits) * 8, uint64_t>());

    auto addrReg = a.allocGpTemp(8);
    a.mov(addrReg, a.argRegs[0]); // save addr as rcx may be destroyed
    a.call(asmjit::imm_ptr(mem::write<T>));

    if constexpr (flags & Store128BitFloat) {
        a.mov(a.argRegs[1], a.loadGpRegisterRead(a.coreVRegs[inst.Rt][1]));
        a.mov(a.argRegs[0], addrReg);
        a.add(a.argRegs[0], 8);
        a.call(asmjit::imm_ptr(mem::write<T>));
    }

    if constexpr (flags & StoreWriteBack) {
        if constexpr (flags & StorePostIndex) {
            a.add(addrReg, imm);
        }

        if (inst.Rn == 31) {
            a.mov(a.loadGpRegisterWrite(a.coreSPMem), addrReg);
        } else {
            a.mov(a.loadGpRegisterWrite(a.coreGPRegs[inst.Rn]), addrReg);
        }
    }

    //TODO
    /*
    if (flags & StoreRelease) {
        std::atomic_thread_fence(std::memory_order_release);
    }
    */
}

void strbImm(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t>(a, inst);
}

void strhImm(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t>(a, inst);
}

void str32Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t>(a, inst);
}

void str64Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t>(a, inst);
}

void str8ImmFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreFloat>(a, inst);
}

void str16ImmFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreFloat>(a, inst);
}

void str32ImmFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreFloat>(a, inst);
}

void str64ImmFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreFloat>(a, inst);
}

void str128Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreFloat | Store128BitFloat>(a, inst);
}

void strbPost(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreWriteBack | StorePostIndex>(a, inst);
}

void strhPost(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreWriteBack | StorePostIndex>(a, inst);
}

void str32Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreWriteBack | StorePostIndex>(a, inst);
}

void str64Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StorePostIndex>(a, inst);
}

void str8PostFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreWriteBack | StorePostIndex | StoreFloat>(a, inst);
}

void str16PostFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreWriteBack | StorePostIndex | StoreFloat>(a, inst);
}

void str32PostFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreWriteBack | StorePostIndex | StoreFloat>(a, inst);
}

void str64PostFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StorePostIndex | StoreFloat>(a, inst);
}

void str128Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StorePostIndex | StoreFloat | Store128BitFloat>(a, inst);
}

void strbPre(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreWriteBack>(a, inst);
}

void strhPre(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreWriteBack>(a, inst);
}

void str32Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreWriteBack>(a, inst);
}

void str64Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack>(a, inst);
}

void str8PreFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreWriteBack | StoreFloat>(a, inst);
}

void str16PreFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreWriteBack | StoreFloat>(a, inst);
}

void str32PreFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreWriteBack | StoreFloat>(a, inst);
}

void str64PreFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StoreFloat>(a, inst);
}

void str128Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StoreFloat | Store128BitFloat>(a, inst);
}

void sturb(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreImmSigned>(a, inst);
}

void sturh(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreImmSigned>(a, inst);
}

void stur32(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreImmSigned>(a, inst);
}

void stur64(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreImmSigned>(a, inst);
}

void stur8Float(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreImmSigned | StoreFloat>(a, inst);
}

void stur16Float(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreImmSigned | StoreFloat>(a, inst);
}

void stur32Float(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreImmSigned | StoreFloat>(a, inst);
}

void stur64Float(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreImmSigned | StoreFloat>(a, inst);
}

void stur128(AArch64EmuAssembler& a, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreImmSigned | StoreFloat | Store128BitFloat>(a, inst);
}

enum LoadFlags {
    LoadWriteBack = 1 << 0,
    LoadImmSigned = 1 << 1,
    LoadPostIndex = 1 << 2,
    LoadExtendResult = 1 << 3,
    LoadExtendTo64 = 1 << 4,
    LoadFloat = 1 << 5,
    Load128BitFloat = 1 << 6,
    LoadAcquire = 1 << 7
};

template <typename T, unsigned flags = 0>
void loadRegImmGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    if (((flags & LoadWriteBack) != 0) && (inst.Rt == inst.Rn) && (inst.Rn != 31)) return;

    constexpr uint64_t numBits = constLog2(sizeof(T));

    uint64_t imm;
    if constexpr (flags & LoadImmSigned) {
        imm = static_cast<uint64_t>(bitRange<12, 20>(inst.bits));
        imm = signExtend<9>(imm);
    } else {
        imm = static_cast<uint64_t>(bitRange<10, 21>(inst.bits));
        imm <<= numBits;
    }

    if (inst.Rn == 31) {
        // TODO check SP alignment
        a.mov(a.argRegs[0], a.loadGpRegisterRead(a.coreSPMem));
    } else {
        a.mov(a.argRegs[0], a.loadGpRegisterRead(a.coreGPRegs[inst.Rn]));
    }

    if constexpr (!(flags & LoadPostIndex)) {
        a.add(a.argRegs[0], imm);
    }

    auto addrReg = a.allocGpTemp(8);
    a.mov(addrReg, a.argRegs[0]); // save addr as rcx may be destroyed
    a.call(asmjit::imm_ptr(mem::read<T>));

    if constexpr (flags & LoadExtendResult) {
        a.mov(a.argRegs[0], asmjit::x86::rax);
        a.call(asmjit::imm_ptr(signExtend<(1 << numBits) * 8, uint64_t>));
        if constexpr (!(flags & LoadExtendTo64)) {
            a.and_(asmjit::x86::rax, bitmask<32, uint64_t>());
        }
    }

    if constexpr (flags & LoadFloat) {
        a.mov(a.loadGpRegisterWrite(a.coreVRegs[inst.Rt][0]), asmjit::x86::rax);
        if constexpr (flags & Load128BitFloat) {
            a.mov(a.argRegs[0], addrReg);
            a.add(a.argRegs[0], 8);
            a.call(asmjit::imm_ptr(mem::read<T>));
            a.mov(a.loadGpRegisterWrite(a.coreVRegs[inst.Rt][1]), asmjit::x86::rax);
        }
    } else {
        a.mov(a.loadGpRegisterWrite(a.coreGPRegs[inst.Rt]), asmjit::x86::rax);
    }

    if constexpr (flags & LoadWriteBack) {
        if constexpr (flags & LoadPostIndex) {
            a.add(addrReg, imm);
        }

        if (inst.Rn == 31) {
            a.mov(a.loadGpRegisterWrite(a.coreSPMem), addrReg);
        } else {
            a.mov(a.loadGpRegisterWrite(a.coreGPRegs[inst.Rn]), addrReg);
        }
    }

    // TODO
    /*
    if (flags & LoadAcquire) {
        std::atomic_thread_fence(std::memory_order_acquire);
    }
    */
}

void ldrbImm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t>(a, inst);
}

void ldrsb32Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadExtendResult>(a, inst);
}

void ldrsb64Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldrhImm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t>(a, inst);
}

void ldrsh32Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadExtendResult>(a, inst);
}

void ldrsh64Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldr32Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t>(a, inst);
}

void ldrswImm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldr64Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t>(a, inst);
}

void ldr8ImmFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadFloat>(a, inst);
}

void ldr16ImmFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadFloat>(a, inst);
}

void ldr32ImmFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadFloat>(a, inst);
}

void ldr64ImmFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadFloat>(a, inst);
}

void ldr128Imm(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadFloat | Load128BitFloat>(a, inst);
}

void ldrbPost(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadPostIndex>(a, inst);
}

void ldrsb32Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadPostIndex | LoadExtendResult>(a, inst);
}

void ldrsb64Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadPostIndex | LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldrhPost(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadPostIndex>(a, inst);
}

void ldrsh32Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadPostIndex | LoadExtendResult>(a, inst);
}

void ldrsh64Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadPostIndex | LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldr32Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadPostIndex>(a, inst);
}

void ldrswPost(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadPostIndex | LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldr64Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack | LoadPostIndex>(a, inst);
}

void ldr8PostFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadPostIndex | LoadFloat>(a, inst);
}

void ldr16PostFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadPostIndex | LoadFloat>(a, inst);
}

void ldr32PostFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadPostIndex | LoadFloat>(a, inst);
}

void ldr64PostFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack | LoadPostIndex | LoadFloat>(a, inst);
}

void ldr128Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadPostIndex | LoadFloat | Load128BitFloat>(a, inst);
}

void ldrbPre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack>(a, inst);
}

void ldrsb32Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadExtendResult>(a, inst);
}

void ldrsb64Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldrhPre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack>(a, inst);
}

void ldrsh32Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadExtendResult>(a, inst);
}

void ldrsh64Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldr32Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack>(a, inst);
}

void ldrswPre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldr64Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack>(a, inst);
}

void ldr8PreFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadFloat>(a, inst);
}

void ldr16PreFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadFloat>(a, inst);
}

void ldr32PreFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadFloat>(a, inst);
}

void ldr64PreFloat(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack | LoadFloat>(a, inst);
}

void ldr128Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadFloat | Load128BitFloat>(a, inst);
}

void ldurb(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadImmSigned>(a, inst);
}

void ldursb32(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadImmSigned | LoadExtendResult>(a, inst);
}

void ldursb64(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadImmSigned | LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldurh(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadImmSigned>(a, inst);
}

void ldursh32(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadImmSigned | LoadExtendResult>(a, inst);
}

void ldursh64(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadImmSigned | LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldur32(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadImmSigned>(a, inst);
}

void ldursw(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadImmSigned | LoadExtendResult | LoadExtendTo64>(a, inst);
}

void ldur64(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadImmSigned>(a, inst);
}

void ldur8Float(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadImmSigned | LoadFloat>(a, inst);
}

void ldur16Float(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadImmSigned | LoadFloat>(a, inst);
}

void ldur32Float(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadImmSigned | LoadFloat>(a, inst);
}

void ldur64Float(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadImmSigned | LoadFloat>(a, inst);
}

void ldur128(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadImmSigned | LoadFloat | Load128BitFloat>(a, inst);
}

// In addition to other implementation defined behavior, for pair instructions, it is implementation defined
// what happens when Rt == Rt2 for loads. We have chosen to also treat it as a nop

template <typename T, unsigned flags = 0>
void storePairGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    // TODO float
    if (((flags & StoreWriteBack) != 0) && ((inst.Rt == inst.Rn) || (inst.Rt2 == inst.Rn)) && (inst.Rn != 31)) return;

    constexpr uint64_t numBits = constLog2(sizeof(T));
    uint64_t imm = static_cast<uint64_t>(inst._15_21);

    imm = signExtend<7>(imm);
    imm <<= numBits;

    if (inst.Rn == 31) {
        // TODO check SP alignment
        a.mov(a.argRegs[0], a.loadGpRegisterRead(a.coreSPMem));
    } else {
        a.mov(a.argRegs[0], a.loadGpRegisterRead(a.coreGPRegs[inst.Rn]));
    }

    if constexpr (!(flags & StorePostIndex)) {
        a.add(a.argRegs[0], imm);
    }
    auto addrReg = a.allocGpTemp(8);

    a.mov(a.argRegs[1], a.loadGpRegisterRead(a.coreGPRegs[inst.Rt]));
    // TODO is this needed?
    a.and_(a.argRegs[1], bitmask<(1 << numBits) * 8, uint64_t>());
    a.mov(addrReg, a.argRegs[0]); // save addr as rcx may be destroyed
    a.call(asmjit::imm_ptr(mem::write<T>));

    a.mov(a.argRegs[0], addrReg);
    a.add(a.argRegs[0], sizeof(T));
    a.mov(a.argRegs[1], a.loadGpRegisterRead(a.coreGPRegs[inst.Rt2]));
    // TODO is this needed?
    a.and_(a.argRegs[1], bitmask<(1 << numBits) * 8, uint64_t>());
    a.call(asmjit::imm_ptr(mem::write<T>));

    if constexpr (flags & StoreWriteBack) {
        if constexpr (flags & StorePostIndex) {
            a.add(addrReg, imm);
        }

        if (inst.Rn == 31) {
            a.mov(a.loadGpRegisterWrite(a.coreSPMem), addrReg);
        } else {
            a.mov(a.loadGpRegisterWrite(a.coreGPRegs[inst.Rn]), addrReg);
        }
    }
}

void stnp32(AArch64EmuAssembler& a, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    storePairGeneric<uint32_t>(a, inst);
}

void stnp64(AArch64EmuAssembler& a, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    storePairGeneric<uint64_t>(a, inst);
}

void stp32Off(AArch64EmuAssembler& a, AArch64Inst inst) {
    storePairGeneric<uint32_t>(a, inst);
}

void stp64Off(AArch64EmuAssembler& a, AArch64Inst inst) {
    storePairGeneric<uint64_t>(a, inst);
}

void stp32Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    storePairGeneric<uint32_t, StoreWriteBack | StorePostIndex>(a, inst);
}

void stp64Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreWriteBack | StorePostIndex>(a, inst);
}

void stp32Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    storePairGeneric<uint32_t, StoreWriteBack>(a, inst);
}

void stp64Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreWriteBack>(a, inst);
}

template <typename T, unsigned flags = 0>
void loadPairGeneric(AArch64EmuAssembler& a, AArch64Inst inst) {
    // TODO float
    if (((flags & StoreWriteBack) != 0) && ((inst.Rt == inst.Rn) || (inst.Rt2 == inst.Rn)) && (inst.Rn != 31)) return;
    if (inst.Rt == inst.Rt2) return;

    uint64_t imm = static_cast<uint64_t>(inst._15_21);
    constexpr uint64_t numBits = constLog2(sizeof(T));

    imm = signExtend<7>(imm);
    imm <<= numBits;

    if (inst.Rn == 31) {
        // TODO check SP alignment
        a.mov(a.argRegs[0], a.loadGpRegisterRead(a.coreSPMem));
    } else {
        a.mov(a.argRegs[0], a.loadGpRegisterRead(a.coreGPRegs[inst.Rn]));
    }

    if constexpr (!(flags & LoadPostIndex)) {
        a.add(a.argRegs[0], imm);
    }
    auto addrReg = a.allocGpTemp(8);

    a.mov(addrReg, a.argRegs[0]); // save addr as rcx may be destroyed
    a.call(asmjit::imm_ptr(mem::read<T>));
    a.mov(a.loadGpRegisterWrite(a.coreGPRegs[inst.Rt]), asmjit::x86::rax);

    a.mov(a.argRegs[0], addrReg);
    a.add(a.argRegs[0], sizeof(T));
    a.call(asmjit::imm_ptr(mem::read<T>));
    a.mov(a.loadGpRegisterWrite(a.coreGPRegs[inst.Rt2]), asmjit::x86::rax);

    if constexpr (flags & LoadWriteBack) {
        if constexpr (flags & LoadPostIndex) {
            a.add(addrReg, imm);
        }

        if (inst.Rn == 31) {
            a.mov(a.loadGpRegisterWrite(a.coreSPMem), addrReg);
        } else {
            a.mov(a.loadGpRegisterWrite(a.coreGPRegs[inst.Rn]), addrReg);
        }
    }
}

void ldnp32(AArch64EmuAssembler& a, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    loadPairGeneric<uint32_t>(a, inst);
}

void ldnp64(AArch64EmuAssembler& a, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    loadPairGeneric<uint64_t>(a, inst);
}

void ldp32Off(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadPairGeneric<uint32_t>(a, inst);
}

void ldp64Off(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadPairGeneric<uint64_t>(a, inst);
}

void ldp32Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadPairGeneric<uint32_t, LoadWriteBack | LoadPostIndex>(a, inst);
}

void ldp64Post(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadWriteBack | LoadPostIndex>(a, inst);
}

void ldp32Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadPairGeneric<uint32_t, LoadWriteBack>(a, inst);
}

void ldp64Pre(AArch64EmuAssembler& a, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadWriteBack>(a, inst);
}


void registerLoadStoreInstructions() {
    // Load/store exclusive
    registerInstruction(instructionID::STXRB, FALLBACK);
    registerInstruction(instructionID::STLXRB, FALLBACK);
    registerInstruction(instructionID::LDXRB, FALLBACK);
    registerInstruction(instructionID::LDAXRB, FALLBACK);
    registerInstruction(instructionID::STLRB, FALLBACK);
    registerInstruction(instructionID::LDARB, FALLBACK);
    registerInstruction(instructionID::STXRH, FALLBACK);
    registerInstruction(instructionID::STLXRH, FALLBACK);
    registerInstruction(instructionID::LDXRH, FALLBACK);
    registerInstruction(instructionID::LDAXRH, FALLBACK);
    registerInstruction(instructionID::STLRH, FALLBACK);
    registerInstruction(instructionID::LDARH, FALLBACK);
    registerInstruction(instructionID::STXR32, FALLBACK);
    registerInstruction(instructionID::STLXR32, FALLBACK);
    registerInstruction(instructionID::STXP32, FALLBACK);
    registerInstruction(instructionID::STLXP32, FALLBACK);
    registerInstruction(instructionID::LDXR32, FALLBACK);
    registerInstruction(instructionID::LDAXR32, FALLBACK);
    registerInstruction(instructionID::LDXP32, FALLBACK);
    registerInstruction(instructionID::LDAXP32, FALLBACK);
    registerInstruction(instructionID::STLR32, FALLBACK);
    registerInstruction(instructionID::LDAR32, FALLBACK);
    registerInstruction(instructionID::STXR64, FALLBACK);
    registerInstruction(instructionID::STLXR64, FALLBACK);
    registerInstruction(instructionID::STXP64, FALLBACK);
    registerInstruction(instructionID::STLXP64, FALLBACK);
    registerInstruction(instructionID::LDXR64, FALLBACK);
    registerInstruction(instructionID::LDAXR64, FALLBACK);
    registerInstruction(instructionID::LDXP64, FALLBACK);
    registerInstruction(instructionID::LDAXP64, FALLBACK);
    registerInstruction(instructionID::STLR64, FALLBACK);
    registerInstruction(instructionID::LDAR64, FALLBACK);
    // Load register (literal)
    registerInstruction(instructionID::LDR32_literal, FALLBACK);
    registerInstruction(instructionID::LDR32_literal_float, FALLBACK);
    registerInstruction(instructionID::LDR64_literal, FALLBACK);
    registerInstruction(instructionID::LDR64_literal_float, FALLBACK);
    registerInstruction(instructionID::LDRSW_literal, FALLBACK);
    registerInstruction(instructionID::LDR128_literal, FALLBACK);
    registerInstruction(instructionID::PRFM_literal, FALLBACK);
    // Load/store no-allocate pair (offset)
    registerInstruction(instructionID::STNP32, FALLBACK);
    registerInstruction(instructionID::LDNP32, FALLBACK);
    registerInstruction(instructionID::STNP32_float, FALLBACK);
    registerInstruction(instructionID::LDNP32_float, FALLBACK);
    registerInstruction(instructionID::STNP64, FALLBACK);
    registerInstruction(instructionID::LDNP64, FALLBACK);
    registerInstruction(instructionID::STNP64_float, FALLBACK);
    registerInstruction(instructionID::LDNP64_float, FALLBACK);
    registerInstruction(instructionID::STNP128, FALLBACK);
    registerInstruction(instructionID::LDNP128, FALLBACK);
    // Load/store register pair (post-indexed)
    registerInstruction(instructionID::STP32_post, FALLBACK);
    registerInstruction(instructionID::LDP32_post, FALLBACK);
    registerInstruction(instructionID::STP32_post_float, FALLBACK);
    registerInstruction(instructionID::LDP32_post_float, FALLBACK);
    registerInstruction(instructionID::LDPSW_post, FALLBACK);
    registerInstruction(instructionID::STP64_post, FALLBACK);
    registerInstruction(instructionID::LDP64_post, ldp64Post);
    registerInstruction(instructionID::STP64_post_float, FALLBACK);
    registerInstruction(instructionID::LDP64_post_float, FALLBACK);
    registerInstruction(instructionID::STP128_post, FALLBACK);
    registerInstruction(instructionID::LDP128_post, FALLBACK);
    // Load/store register pair (offset)
    registerInstruction(instructionID::STP32_offset, FALLBACK);
    registerInstruction(instructionID::LDP32_offset, FALLBACK);
    registerInstruction(instructionID::STP32_offset_float, FALLBACK);
    registerInstruction(instructionID::LDP32_offset_float, FALLBACK);
    registerInstruction(instructionID::LDPSW_offset, FALLBACK);
    registerInstruction(instructionID::STP64_offset, FALLBACK);
    registerInstruction(instructionID::LDP64_offset, FALLBACK);
    registerInstruction(instructionID::STP64_offset_float, FALLBACK);
    registerInstruction(instructionID::LDP64_offset_float, FALLBACK);
    registerInstruction(instructionID::STP128_offset, FALLBACK);
    registerInstruction(instructionID::LDP128_offset, FALLBACK);
    // Load/store register pair (pre-indexed)
    registerInstruction(instructionID::STP32_pre, FALLBACK);
    registerInstruction(instructionID::LDP32_pre, FALLBACK);
    registerInstruction(instructionID::STP32_pre_float, FALLBACK);
    registerInstruction(instructionID::LDP32_pre_float, FALLBACK);
    registerInstruction(instructionID::LDPSW_pre, FALLBACK);
    registerInstruction(instructionID::STP64_pre, stp64Pre);
    registerInstruction(instructionID::LDP64_pre, FALLBACK);
    registerInstruction(instructionID::STP64_pre_float, FALLBACK);
    registerInstruction(instructionID::LDP64_pre_float, FALLBACK);
    registerInstruction(instructionID::STP128_pre, FALLBACK);
    registerInstruction(instructionID::LDP128_pre, FALLBACK);
    // Load/store register (unscaled immediate)
    registerInstruction(instructionID::STURB, FALLBACK);
    registerInstruction(instructionID::LDURB, FALLBACK);
    registerInstruction(instructionID::LDURSB64, FALLBACK);
    registerInstruction(instructionID::LDURSB32, FALLBACK);
    registerInstruction(instructionID::STUR8_float, FALLBACK);
    registerInstruction(instructionID::LDUR8_float, FALLBACK);
    registerInstruction(instructionID::STUR128, FALLBACK);
    registerInstruction(instructionID::LDUR128, FALLBACK);
    registerInstruction(instructionID::STURH, FALLBACK);
    registerInstruction(instructionID::LDURH, FALLBACK);
    registerInstruction(instructionID::LDURSH64, FALLBACK);
    registerInstruction(instructionID::LDURSH32, FALLBACK);
    registerInstruction(instructionID::STUR16_float, FALLBACK);
    registerInstruction(instructionID::LDUR16_float, FALLBACK);
    registerInstruction(instructionID::STUR32, FALLBACK);
    registerInstruction(instructionID::LDUR32, FALLBACK);
    registerInstruction(instructionID::LDURSW, FALLBACK);
    registerInstruction(instructionID::STUR32_float, FALLBACK);
    registerInstruction(instructionID::LDUR32_float, FALLBACK);
    registerInstruction(instructionID::STUR64, FALLBACK);
    registerInstruction(instructionID::LDUR64, FALLBACK);
    registerInstruction(instructionID::PRFM_unscaled, FALLBACK);
    registerInstruction(instructionID::STUR64_float, FALLBACK);
    registerInstruction(instructionID::LDUR64_float, FALLBACK);
    // Load/store register (immediate post-indexed)
    registerInstruction(instructionID::STRB_post, FALLBACK);
    registerInstruction(instructionID::LDRB_post, FALLBACK);
    registerInstruction(instructionID::LDRSB64_post, FALLBACK);
    registerInstruction(instructionID::LDRSB32_post, FALLBACK);
    registerInstruction(instructionID::STR8_post_float, FALLBACK);
    registerInstruction(instructionID::LDR8_post_float, FALLBACK);
    registerInstruction(instructionID::STR128_post, FALLBACK);
    registerInstruction(instructionID::LDR128_post, FALLBACK);
    registerInstruction(instructionID::STRH_post, FALLBACK);
    registerInstruction(instructionID::LDRH_post, FALLBACK);
    registerInstruction(instructionID::LDRSH64_post, FALLBACK);
    registerInstruction(instructionID::LDRSH32_post, FALLBACK);
    registerInstruction(instructionID::STR16_post_float, FALLBACK);
    registerInstruction(instructionID::LDR16_post_float, FALLBACK);
    registerInstruction(instructionID::STR32_post, FALLBACK);
    registerInstruction(instructionID::LDR32_post, FALLBACK);
    registerInstruction(instructionID::LDRSW_post, FALLBACK);
    registerInstruction(instructionID::STR32_post_float, FALLBACK);
    registerInstruction(instructionID::LDR32_post_float, FALLBACK);
    registerInstruction(instructionID::STR64_post, FALLBACK);
    registerInstruction(instructionID::LDR64_post, FALLBACK);
    registerInstruction(instructionID::STR64_post_float, FALLBACK);
    registerInstruction(instructionID::LDR64_post_float, FALLBACK);
    // Load/store register (unprivileged)
    registerInstruction(instructionID::STTRB, FALLBACK);
    registerInstruction(instructionID::LDTRB, FALLBACK);
    registerInstruction(instructionID::LDTRSB64, FALLBACK);
    registerInstruction(instructionID::LDTRSB32, FALLBACK);
    registerInstruction(instructionID::STTRH, FALLBACK);
    registerInstruction(instructionID::LDTRH, FALLBACK);
    registerInstruction(instructionID::LDTRSH64, FALLBACK);
    registerInstruction(instructionID::LDTRSH32, FALLBACK);
    registerInstruction(instructionID::STTR32, FALLBACK);
    registerInstruction(instructionID::LDTR32, FALLBACK);
    registerInstruction(instructionID::LDTRSW, FALLBACK);
    registerInstruction(instructionID::STTR64, FALLBACK);
    registerInstruction(instructionID::LDTR64, FALLBACK);
    // Load/store register (immediate pre-indexed)
    registerInstruction(instructionID::STRB_pre, FALLBACK);
    registerInstruction(instructionID::LDRB_pre, FALLBACK);
    registerInstruction(instructionID::LDRSB64_pre, FALLBACK);
    registerInstruction(instructionID::LDRSB32_pre, FALLBACK);
    registerInstruction(instructionID::STR8_pre_float, FALLBACK);
    registerInstruction(instructionID::LDR8_pre_float, FALLBACK);
    registerInstruction(instructionID::STR128_pre, FALLBACK);
    registerInstruction(instructionID::LDR128_pre, FALLBACK);
    registerInstruction(instructionID::STRH_pre, FALLBACK);
    registerInstruction(instructionID::LDRH_pre, FALLBACK);
    registerInstruction(instructionID::LDRSH64_pre, FALLBACK);
    registerInstruction(instructionID::LDRSH32_pre, FALLBACK);
    registerInstruction(instructionID::STR16_pre_float, FALLBACK);
    registerInstruction(instructionID::LDR16_pre_float, FALLBACK);
    registerInstruction(instructionID::STR32_pre, FALLBACK);
    registerInstruction(instructionID::LDR32_pre, FALLBACK);
    registerInstruction(instructionID::LDRSW_pre, FALLBACK);
    registerInstruction(instructionID::STR32_pre_float, FALLBACK);
    registerInstruction(instructionID::LDR32_pre_float, FALLBACK);
    registerInstruction(instructionID::STR64_pre, FALLBACK);
    registerInstruction(instructionID::LDR64_pre, FALLBACK);
    registerInstruction(instructionID::STR64_pre_float, FALLBACK);
    registerInstruction(instructionID::LDR64_pre_float, FALLBACK);
    // Load/store register (register offset)
    registerInstruction(instructionID::STRB_reg, FALLBACK);
    registerInstruction(instructionID::LDRB_reg, FALLBACK);
    registerInstruction(instructionID::LDRSB64_reg, FALLBACK);
    registerInstruction(instructionID::LDRSB32_reg, FALLBACK);
    registerInstruction(instructionID::STR8_reg_float, FALLBACK);
    registerInstruction(instructionID::LDR8_reg_float, FALLBACK);
    registerInstruction(instructionID::STR128_reg, FALLBACK);
    registerInstruction(instructionID::LDR128_reg, FALLBACK);
    registerInstruction(instructionID::STRH_reg, FALLBACK);
    registerInstruction(instructionID::LDRH_reg, FALLBACK);
    registerInstruction(instructionID::LDRSH64_reg, FALLBACK);
    registerInstruction(instructionID::LDRSH32_reg, FALLBACK);
    registerInstruction(instructionID::STR16_reg_float, FALLBACK);
    registerInstruction(instructionID::LDR16_reg_float, FALLBACK);
    registerInstruction(instructionID::STR32_reg, FALLBACK);
    registerInstruction(instructionID::LDR32_reg, FALLBACK);
    registerInstruction(instructionID::LDRSW_reg, FALLBACK);
    registerInstruction(instructionID::STR32_reg_float, FALLBACK);
    registerInstruction(instructionID::LDR32_reg_float, FALLBACK);
    registerInstruction(instructionID::STR64_reg, FALLBACK);
    registerInstruction(instructionID::LDR64_reg, FALLBACK);
    registerInstruction(instructionID::PRFM_reg, FALLBACK);
    registerInstruction(instructionID::STR64_reg_float, FALLBACK);
    registerInstruction(instructionID::LDR64_reg_float, FALLBACK);
    // Load/store register (unsigned immediate)
    registerInstruction(instructionID::STRB_imm, FALLBACK);
    registerInstruction(instructionID::LDRB_imm, FALLBACK);
    registerInstruction(instructionID::LDRSB64_imm, FALLBACK);
    registerInstruction(instructionID::LDRSB32_imm, FALLBACK);
    registerInstruction(instructionID::STR8_imm_float, FALLBACK);
    registerInstruction(instructionID::LDR8_imm_float, FALLBACK);
    registerInstruction(instructionID::STR128_imm, FALLBACK);
    registerInstruction(instructionID::LDR128_imm, FALLBACK);
    registerInstruction(instructionID::STRH_imm, FALLBACK);
    registerInstruction(instructionID::LDRH_imm, FALLBACK);
    registerInstruction(instructionID::LDRSH64_imm, FALLBACK);
    registerInstruction(instructionID::LDRSH32_imm, FALLBACK);
    registerInstruction(instructionID::STR16_imm_float, FALLBACK);
    registerInstruction(instructionID::LDR16_imm_float, FALLBACK);
    registerInstruction(instructionID::STR32_imm, str32Imm);
    registerInstruction(instructionID::LDR32_imm, ldr32Imm);
    registerInstruction(instructionID::LDRSW_imm, FALLBACK);
    registerInstruction(instructionID::STR32_imm_float, FALLBACK);
    registerInstruction(instructionID::LDR32_imm_float, FALLBACK);
    registerInstruction(instructionID::STR64_imm, str64Imm);
    registerInstruction(instructionID::LDR64_imm, ldr64Imm);
    registerInstruction(instructionID::PRFM_imm, FALLBACK);
    registerInstruction(instructionID::STR64_imm_float, FALLBACK);
    registerInstruction(instructionID::LDR64_imm_float, FALLBACK);
}

} // namespace jit

} // namespace cpu