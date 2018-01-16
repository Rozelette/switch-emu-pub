#include <atomic>

#include "cpu.h"
#include "bitutils.h"
#include "instutils.h"
#include "mem.h"
#include "unreachable.h"
#include "log.h"
#include "interpreter.h"

namespace cpu {

namespace interpreter {

// It is implementation defined what happens with writeback when Rt == Rn and Rn != 31.
// For the interpreter, we have decided to execute the instruction as normal
// as it is probably faster than actually checking

enum StoreFlags {
    StoreWriteBack   = 1 << 0,
    StoreImmSigned   = 1 << 1,
    StorePostIndex   = 1 << 2,
    StoreFloat       = 1 << 3,
    Store128BitFloat = 1 << 4,
    StoreRelease     = 1 << 5,
    StoreExclusive   = 1 << 6
};

template <typename T, unsigned flags = 0>
void storeRegImmGeneric(Core* core, AArch64Inst inst) {
    constexpr uint64_t numBits = (flags & Store128BitFloat) ? 4 : constLog2(sizeof(T));

    uint64_t imm = 0;
    if constexpr (flags & StoreImmSigned) {
        imm = inst._12_20;
        imm = signExtend<9>(imm);
    } else if constexpr (!(flags & StoreExclusive) && !(flags & StoreRelease)) {
        imm = inst._10_21;
        imm <<= numBits;
    }

    uint64_t addr;
    if (inst.Rn == 31) {
        // TODO check SP alignment
        addr = core->SP;
    } else {
        addr = core->GPRegs[inst.Rn];
    }

    if constexpr (!(flags & StorePostIndex)) {
        addr += imm;
    }

    uint64_t data = 0;
    if constexpr (flags & StoreFloat) {
        data = core->VRegs[inst.Rt].D[0];
    } else {
        data = core->GPRegs[inst.Rt];
    }
    uint64_t mask = 0;
    if constexpr (flags & Store128BitFloat) {
        mask = bitmask<64, uint64_t>();
    } else {
        mask = bitmask<(1 << numBits) * 8, uint64_t>();
    }
    data &= mask;

    if constexpr (flags & StoreExclusive) {
        bool exclusiveStatus = mem::writeAtomic(addr, static_cast<T>(data), static_cast<T>(core->exclusiveData0));
        core->GPRegs[inst.Rs] = (exclusiveStatus) ? 0 : 1;
    } else {
        mem::write(addr, static_cast<T>(data));
    }

    if constexpr (flags & Store128BitFloat) {
        data = core->VRegs[inst.Rt].D[1];
        mem::write(addr + 8, static_cast<T>(data));
    }

    if constexpr (flags & StoreWriteBack) {
        if constexpr (flags & StorePostIndex) {
            addr += imm;
        }

        if (inst.Rn == 31) {
            core->SP = addr;
        } else {
            core->GPRegs[inst.Rn] = addr;
        }
    }

    if constexpr (flags & StoreRelease) {
        // TODO is this correct?
        std::atomic_thread_fence(std::memory_order_release);
    }
}

void strbImm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t>(core, inst);
}

void strhImm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t>(core, inst);
}

void str32Imm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t>(core, inst);
}

void str64Imm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t>(core, inst);
}

void str8ImmFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreFloat>(core, inst);
}

void str16ImmFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreFloat>(core, inst);
}

void str32ImmFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreFloat>(core, inst);
}

void str64ImmFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreFloat>(core, inst);
}

void str128Imm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreFloat | Store128BitFloat>(core, inst);
}

void strbPost(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreWriteBack | StorePostIndex | StoreImmSigned>(core, inst);
}

void strhPost(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreWriteBack | StorePostIndex | StoreImmSigned>(core, inst);
}

void str32Post(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreWriteBack | StorePostIndex | StoreImmSigned>(core, inst);
}

void str64Post(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StorePostIndex | StoreImmSigned>(core, inst);
}

void str8PostFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreWriteBack | StorePostIndex | StoreFloat | StoreImmSigned>(core, inst);
}

void str16PostFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreWriteBack | StorePostIndex | StoreFloat | StoreImmSigned>(core, inst);
}

void str32PostFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreWriteBack | StorePostIndex | StoreFloat | StoreImmSigned>(core, inst);
}

void str64PostFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StorePostIndex | StoreFloat | StoreImmSigned>(core, inst);
}

void str128Post(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StorePostIndex | StoreFloat | Store128BitFloat | StoreImmSigned>(core, inst);
}

void strbPre(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreWriteBack | StoreImmSigned>(core, inst);
}

void strhPre(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreWriteBack | StoreImmSigned>(core, inst);
}

void str32Pre(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreWriteBack | StoreImmSigned>(core, inst);
}

void str64Pre(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StoreImmSigned>(core, inst);
}

void str8PreFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreWriteBack | StoreFloat | StoreImmSigned>(core, inst);
}

void str16PreFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreWriteBack | StoreFloat | StoreImmSigned>(core, inst);
}

void str32PreFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreWriteBack | StoreFloat | StoreImmSigned>(core, inst);
}

void str64PreFloat(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StoreFloat | StoreImmSigned>(core, inst);
}

void str128Pre(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreWriteBack | StoreFloat | Store128BitFloat | StoreImmSigned>(core, inst);
}

void sturb(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreImmSigned>(core, inst);
}

void sturh(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreImmSigned>(core, inst);
}

void stur32(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreImmSigned>(core, inst);
}

void stur64(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreImmSigned>(core, inst);
}

void stur8Float(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreImmSigned | StoreFloat>(core, inst);
}

void stur16Float(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreImmSigned | StoreFloat>(core, inst);
}

void stur32Float(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreImmSigned | StoreFloat>(core, inst);
}

void stur64Float(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreImmSigned | StoreFloat>(core, inst);
}

void stur128(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreImmSigned | StoreFloat | Store128BitFloat>(core, inst);
}

void stxrbImm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreExclusive>(core, inst);
}

void stlxrbImm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreRelease | StoreExclusive>(core, inst);
}

void stlrbImm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint8_t, StoreRelease>(core, inst);
}

void stxrhImm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreExclusive>(core, inst);
}

void stlxrhImm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreRelease | StoreExclusive>(core, inst);
}

void stlrhImm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint16_t, StoreRelease>(core, inst);
}

void stxr32Imm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreExclusive>(core, inst);
}

void stlxr32Imm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreRelease | StoreExclusive>(core, inst);
}

void stlr32Imm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint32_t, StoreRelease>(core, inst);
}

void stxr64Imm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreExclusive>(core, inst);
}

void stlxr64Imm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreRelease | StoreExclusive>(core, inst);
}

void stlr64Imm(Core* core, AArch64Inst inst) {
    storeRegImmGeneric<uint64_t, StoreRelease>(core, inst);
}

enum LoadFlags {
    LoadWriteBack    = 1 << 0,
    LoadImmSigned    = 1 << 1,
    LoadPostIndex    = 1 << 2,
    LoadExtendResult = 1 << 3,
    LoadExtendTo64   = 1 << 4,
    LoadFloat        = 1 << 5,
    Load128BitFloat  = 1 << 6,
    LoadAcquire      = 1 << 7,
    LoadExclusive    = 1 << 8
};

template <typename T, unsigned flags = 0>
void loadRegImmGeneric(Core* core, AArch64Inst inst) {
    constexpr uint64_t numBits = (flags & Load128BitFloat) ? 4 : constLog2(sizeof(T));

    uint64_t imm = 0;
    if constexpr (flags & LoadImmSigned) {
        imm = inst._12_20;
        imm = signExtend<9>(imm);
    } else if constexpr (!(flags & LoadExclusive) && !(flags & LoadAcquire)) {
        imm = inst._10_21;
        imm <<= numBits;
    }

    uint64_t addr;
    if (inst.Rn == 31) {
        // TODO check SP alignment
        addr = core->SP;
    } else {
        addr = core->GPRegs[inst.Rn];
    }

    if constexpr (!(flags & LoadPostIndex)) {
        addr += imm;
    }

    uint64_t data = mem::read<T>(addr);

    if constexpr (flags & LoadExtendResult) {
        data = signExtend<(1 << numBits) * 8>(data);
        if constexpr (!(flags & LoadExtendTo64)) {
            data &= bitmask<32, uint64_t>();
        }
    }

    if constexpr (flags & LoadFloat) {
        core->VRegs[inst.Rt].D[0] = data;

        if constexpr (flags & Load128BitFloat) {
            data = mem::read<T>(addr + 8);
            core->VRegs[inst.Rt].D[1] = data;
        }
    } else {
        core->GPRegs[inst.Rt] = data;
    }

    if constexpr (flags & LoadWriteBack) {
        if constexpr (flags & LoadPostIndex) {
            addr += imm;
        }

        if (inst.Rn == 31) {
            core->SP = addr;
        } else {
            core->GPRegs[inst.Rn] = addr;
        }
    }

    if constexpr (flags & LoadAcquire) {
        // TODO is this correct?
        std::atomic_thread_fence(std::memory_order_acquire);
    }

    if constexpr (flags & LoadExclusive) {
        core->exclusiveAccess = true;
        core->exclusiveAccessAddr = addr;
        if constexpr (flags & LoadFloat) {
            if constexpr (flags & Load128BitFloat) {
                core->exclusiveData0 = core->VRegs[inst.Rt].D[0];
                core->exclusiveData1 = core->VRegs[inst.Rt].D[1];
            } else {
                core->exclusiveData0 = core->VRegs[inst.Rt].D[0];
                core->exclusiveData1 = 0;
            }
        } else {
            core->exclusiveData0 = core->GPRegs[inst.Rt];
            core->exclusiveData1 = 0;
        }
    }
}

void ldrbImm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t>(core, inst);
}

void ldrsb32Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadExtendResult>(core, inst);
}

void ldrsb64Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadExtendResult | LoadExtendTo64>(core, inst);
}

void ldrhImm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t>(core, inst);
}

void ldrsh32Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadExtendResult>(core, inst);
}

void ldrsh64Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadExtendResult | LoadExtendTo64>(core, inst);
}

void ldr32Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t>(core, inst);
}

void ldrswImm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadExtendResult | LoadExtendTo64>(core, inst);
}

void ldr64Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t>(core, inst);
}

void ldr8ImmFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadFloat>(core, inst);
}

void ldr16ImmFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadFloat>(core, inst);
}

void ldr32ImmFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadFloat>(core, inst);
}

void ldr64ImmFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadFloat>(core, inst);
}

void ldr128Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadFloat | Load128BitFloat>(core, inst);
}

void ldrbPost(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadPostIndex | LoadImmSigned>(core, inst);
}

void ldrsb32Post(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadPostIndex | LoadExtendResult | LoadImmSigned>(core, inst);
}

void ldrsb64Post(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadPostIndex | LoadExtendResult | LoadExtendTo64 | LoadImmSigned>(core, inst);
}

void ldrhPost(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadPostIndex | LoadImmSigned>(core, inst);
}

void ldrsh32Post(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadPostIndex | LoadExtendResult | LoadImmSigned>(core, inst);
}

void ldrsh64Post(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadPostIndex | LoadExtendResult | LoadExtendTo64 | LoadImmSigned>(core, inst);
}

void ldr32Post(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadPostIndex | LoadImmSigned>(core, inst);
}

void ldrswPost(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadPostIndex | LoadExtendResult | LoadExtendTo64 | LoadImmSigned>(core, inst);
}

void ldr64Post(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack | LoadPostIndex | LoadImmSigned>(core, inst);
}

void ldr8PostFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadPostIndex | LoadFloat | LoadImmSigned>(core, inst);
}

void ldr16PostFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadPostIndex | LoadFloat | LoadImmSigned>(core, inst);
}

void ldr32PostFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadPostIndex | LoadFloat | LoadImmSigned>(core, inst);
}

void ldr64PostFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack | LoadPostIndex | LoadFloat | LoadImmSigned>(core, inst);
}

void ldr128Post(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack | LoadPostIndex | LoadFloat | Load128BitFloat | LoadImmSigned>(core, inst);
}

void ldrbPre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadImmSigned>(core, inst);
}

void ldrsb32Pre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadExtendResult | LoadImmSigned>(core, inst);
}

void ldrsb64Pre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadExtendResult | LoadExtendTo64 | LoadImmSigned>(core, inst);
}

void ldrhPre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadImmSigned>(core, inst);
}

void ldrsh32Pre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadExtendResult | LoadImmSigned>(core, inst);
}

void ldrsh64Pre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadExtendResult | LoadExtendTo64 | LoadImmSigned>(core, inst);
}

void ldr32Pre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadImmSigned>(core, inst);
}

void ldrswPre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadExtendResult | LoadExtendTo64 | LoadImmSigned>(core, inst);
}

void ldr64Pre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack | LoadImmSigned>(core, inst);
}

void ldr8PreFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadWriteBack | LoadFloat | LoadImmSigned>(core, inst);
}

void ldr16PreFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadWriteBack | LoadFloat | LoadImmSigned>(core, inst);
}

void ldr32PreFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadWriteBack | LoadFloat | LoadImmSigned>(core, inst);
}

void ldr64PreFloat(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack | LoadFloat | LoadImmSigned>(core, inst);
}

void ldr128Pre(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadWriteBack | LoadFloat | Load128BitFloat | LoadImmSigned>(core, inst);
}

void ldurb(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadImmSigned>(core, inst);
}

void ldursb32(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadImmSigned | LoadExtendResult>(core, inst);
}

void ldursb64(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadImmSigned | LoadExtendResult | LoadExtendTo64>(core, inst);
}

void ldurh(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadImmSigned>(core, inst);
}

void ldursh32(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadImmSigned | LoadExtendResult>(core, inst);
}

void ldursh64(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadImmSigned | LoadExtendResult | LoadExtendTo64>(core, inst);
}

void ldur32(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadImmSigned>(core, inst);
}

void ldursw(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadImmSigned | LoadExtendResult | LoadExtendTo64>(core, inst);
}

void ldur64(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadImmSigned>(core, inst);
}

void ldur8Float(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadImmSigned | LoadFloat>(core, inst);
}

void ldur16Float(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadImmSigned | LoadFloat>(core, inst);
}

void ldur32Float(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadImmSigned | LoadFloat>(core, inst);
}

void ldur64Float(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadImmSigned | LoadFloat>(core, inst);
}

void ldur128(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadImmSigned | LoadFloat | Load128BitFloat>(core, inst);
}

void ldxrbImm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadExclusive>(core, inst);
}

void ldaxrbImm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadAcquire | LoadExclusive>(core, inst);
}

void ldarbImm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint8_t, LoadAcquire>(core, inst);
}

void ldxrhImm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadExclusive>(core, inst);
}

void ldaxrhImm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadAcquire | LoadExclusive>(core, inst);
}

void ldarhImm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint16_t, LoadAcquire>(core, inst);
}

void ldxr32Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadExclusive>(core, inst);
}

void ldaxr32Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadAcquire | LoadExclusive>(core, inst);
}

void ldar32Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint32_t, LoadAcquire>(core, inst);
}

void ldxr64Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadExclusive>(core, inst);
}

void ldaxr64Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadAcquire | LoadExclusive>(core, inst);
}

void ldar64Imm(Core* core, AArch64Inst inst) {
    loadRegImmGeneric<uint64_t, LoadAcquire>(core, inst);
}

// In addition to other implementation defined behavior, for pair instructions, it is implementation defined
// what happens when Rt == Rt2. We have chosen to also perform the instruction as normal

template <typename T, unsigned flags = 0>
void storePairGeneric(Core* core, AArch64Inst inst) {
    constexpr uint64_t numBits = (flags & Store128BitFloat) ? 4 : constLog2(sizeof(T));
    uint64_t imm = (flags & StoreExclusive) ? 0 : static_cast<uint64_t>(inst._15_21);

    imm = signExtend<7>(imm);
    imm <<= numBits;

    uint64_t addr;
    if (inst.Rn == 31) {
        // TODO check SP alignment
        addr = core->SP;
    } else {
        addr = core->GPRegs[inst.Rn];
    }

    if constexpr (!(flags & StorePostIndex)) {
        addr += imm;
    }

    if constexpr (flags & StoreFloat) {
        if constexpr (flags & Store128BitFloat) {
            uint64_t data = core->VRegs[inst.Rt].D[0];
            mem::write(addr, data);
            data = core->VRegs[inst.Rt].D[1];
            mem::write(addr + 8, data);
            data = core->VRegs[inst.Rt2].D[0];
            mem::write(addr + 16, data);
            data = core->VRegs[inst.Rt2].D[1];
            mem::write(addr + 24, data);
        } else {
            uint64_t data1 = core->VRegs[inst.Rt].D[0];
            uint64_t data2 = core->VRegs[inst.Rt2].D[0];

            mem::write(addr, static_cast<T>(data1));
            mem::write(addr + sizeof(T), static_cast<T>(data2));
        }
    } else if constexpr (flags & StoreExclusive) {
        if constexpr (sizeof(T) == 4) {
            uint64_t data = core->GPRegs[inst.Rt] | (core->GPRegs[inst.Rt2] << 32);
            bool exclusiveStatus = mem::writeAtomic(addr, data, core->exclusiveData0);
            core->GPRegs[inst.Rs] = (exclusiveStatus) ? 0 : 1;
        } else {
            uint64_t data1 = core->GPRegs[inst.Rt];
            uint64_t data2 = core->GPRegs[inst.Rt2];
            bool exclusiveStatus = mem::writeAtomic128(addr, data1, data2, core->exclusiveData0, core->exclusiveData1);
            core->GPRegs[inst.Rs] = (exclusiveStatus) ? 0 : 1;
        }
    } else {
        uint64_t data1 = core->GPRegs[inst.Rt];
        uint64_t data2 = core->GPRegs[inst.Rt2];

        mem::write(addr, static_cast<T>(data1));
        mem::write(addr + sizeof(T), static_cast<T>(data2));
    }

    if constexpr (flags & StoreWriteBack) {
        if constexpr (flags & StorePostIndex) {
            addr += imm;
        }

        if (inst.Rn == 31) {
            core->SP = addr;
        } else {
            core->GPRegs[inst.Rn] = addr;
        }
    }

    if constexpr (flags & StoreRelease) {
        // TODO is this correct?
        std::atomic_thread_fence(std::memory_order_release);
    }
}

void stnp32(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    storePairGeneric<uint32_t>(core, inst);
}

void stnp64(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    storePairGeneric<uint64_t>(core, inst);
}

void stp32Off(Core* core, AArch64Inst inst) {
    storePairGeneric<uint32_t>(core, inst);
}

void stp64Off(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t>(core, inst);
}

void stp32Post(Core* core, AArch64Inst inst) {
    storePairGeneric<uint32_t, StoreWriteBack | StorePostIndex>(core, inst);
}

void stp64Post(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreWriteBack | StorePostIndex>(core, inst);
}

void stp32Pre(Core* core, AArch64Inst inst) {
    storePairGeneric<uint32_t, StoreWriteBack>(core, inst);
}

void stp64Pre(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreWriteBack>(core, inst);
}

void stnp32Float(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    storePairGeneric<uint32_t, StoreFloat>(core, inst);
}

void stnp64Float(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    storePairGeneric<uint64_t, StoreFloat>(core, inst);
}

void stnp128Float(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    storePairGeneric<uint64_t, StoreFloat | Store128BitFloat>(core, inst);
}

void stp32OffFloat(Core* core, AArch64Inst inst) {
    storePairGeneric<uint32_t, StoreFloat>(core, inst);
}

void stp64OffFloat(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreFloat>(core, inst);
}

void stp128OffFloat(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreFloat | Store128BitFloat>(core, inst);
}

void stp32PostFloat(Core* core, AArch64Inst inst) {
    storePairGeneric<uint32_t, StoreWriteBack | StorePostIndex | StoreFloat>(core, inst);
}

void stp64PostFloat(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreWriteBack | StorePostIndex | StoreFloat>(core, inst);
}

void stp128PostFloat(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreWriteBack | StorePostIndex | StoreFloat | Store128BitFloat>(core, inst);
}


void stp32PreFloat(Core* core, AArch64Inst inst) {
    storePairGeneric<uint32_t, StoreWriteBack | StoreFloat>(core, inst);
}

void stp64PreFloat(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreWriteBack | StoreFloat>(core, inst);
}

void stp128PreFloat(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreWriteBack | StoreFloat | Store128BitFloat>(core, inst);
}

void stxp32(Core* core, AArch64Inst inst) {
    storePairGeneric<uint32_t,  StoreExclusive>(core, inst);
}

void stlxp32(Core* core, AArch64Inst inst) {
    storePairGeneric<uint32_t, StoreExclusive | StoreRelease>(core, inst);
}

void stxp64(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreExclusive>(core, inst);
}

void stlxp64(Core* core, AArch64Inst inst) {
    storePairGeneric<uint64_t, StoreExclusive | StoreRelease>(core, inst);
}

template <typename T, unsigned flags = 0>
void loadPairGeneric(Core* core, AArch64Inst inst) {
    uint64_t imm = (flags & LoadExclusive)? 0 : static_cast<uint64_t>(inst._15_21);
    constexpr uint64_t numBits = (flags & Load128BitFloat) ? 4 : constLog2(sizeof(T));

    imm = signExtend<7>(imm);

    uint64_t addr;
    imm <<= numBits;

    if (inst.Rn == 31) {
        // TODO check SP alignment
        addr = core->SP;
    } else {
        addr = core->GPRegs[inst.Rn];
    }

    if constexpr (!(flags & LoadPostIndex)) {
        addr += imm;
    }

    if constexpr (flags & LoadFloat) {
        if constexpr (flags & Load128BitFloat) {
            core->VRegs[inst.Rt].D[0] = mem::read<T>(addr);
            core->VRegs[inst.Rt].D[1] = mem::read<T>(addr + 8);
            core->VRegs[inst.Rt2].D[0] = mem::read<T>(addr + 16);
            core->VRegs[inst.Rt2].D[1] = mem::read<T>(addr + 24);
        } else {
            uint64_t data1;
            uint64_t data2;

            data1 = mem::read<T>(addr);
            data2 = mem::read<T>(addr + sizeof(T));

            core->VRegs[inst.Rt].D[0] = data1;
            core->VRegs[inst.Rt2].D[0] = data2;
        }
    } else {
        uint64_t data1;
        uint64_t data2;

        data1 = mem::read<T>(addr);
        data2 = mem::read<T>(addr + sizeof(T));

        core->GPRegs[inst.Rt] = data1;
        core->GPRegs[inst.Rt2] = data2;

        if constexpr (flags & LoadExclusive) {
            core->exclusiveAccess = true;
            core->exclusiveAccessAddr = addr;
            if constexpr (sizeof(T) == 4) {
                core->exclusiveData0 = data1 | (data2 << 32);
                core->exclusiveData1 = 0;
            } else {
                core->exclusiveData0 = data1;
                core->exclusiveData1 = data2;
            }
        }
    }

    if constexpr (flags & LoadWriteBack) {
        if constexpr (flags & LoadPostIndex) {
            addr += imm;
        }

        if (inst.Rn == 31) {
            core->SP = addr;
        } else {
            core->GPRegs[inst.Rn] = addr;
        }
    }

    if constexpr (flags & LoadAcquire) {
        // TODO is this correct?
        std::atomic_thread_fence(std::memory_order_acquire);
    }
}

void ldnp32(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    loadPairGeneric<uint32_t>(core, inst);
}

void ldnp64(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    loadPairGeneric<uint64_t>(core, inst);
}

void ldp32Off(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint32_t>(core, inst);
}

void ldp64Off(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t>(core, inst);
}

void ldp32Post(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint32_t, LoadWriteBack | LoadPostIndex>(core, inst);
}

void ldp64Post(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadWriteBack | LoadPostIndex>(core, inst);
}

void ldp32Pre(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint32_t, LoadWriteBack>(core, inst);
}

void ldp64Pre(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadWriteBack>(core, inst);
}

void ldnp32Float(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    loadPairGeneric<uint32_t, LoadFloat>(core, inst);
}

void ldnp64Float(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    loadPairGeneric<uint64_t, LoadFloat>(core, inst);
}

void ldnp128Float(Core* core, AArch64Inst inst) {
    // TODO how exactly is this different from a regular offset? What does "non-temporal" mean?
    loadPairGeneric<uint64_t, LoadFloat | Load128BitFloat>(core, inst);
}

void ldp32OffFloat(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint32_t, LoadFloat>(core, inst);
}

void ldp64OffFloat(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadFloat>(core, inst);
}

void ldp128OffFloat(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadFloat | Load128BitFloat>(core, inst);
}

void ldp32PostFloat(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint32_t, LoadWriteBack | LoadPostIndex | LoadFloat>(core, inst);
}

void ldp64PostFloat(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadWriteBack | LoadPostIndex | LoadFloat>(core, inst);
}

void ldp128PostFloat(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadWriteBack | LoadPostIndex | LoadFloat | Load128BitFloat>(core, inst);
}

void ldp32PreFloat(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint32_t, LoadWriteBack | LoadFloat>(core, inst);
}

void ldp64PreFloat(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadWriteBack | LoadFloat>(core, inst);
}

void ldp128PreFloat(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadWriteBack | LoadFloat | Load128BitFloat>(core, inst);
}

void ldxp32(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint32_t,  LoadExclusive>(core, inst);
}

void ldaxp32(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint32_t, LoadExclusive | LoadAcquire>(core, inst);
}

void ldxp64(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadExclusive>(core, inst);
}

void ldaxp64(Core* core, AArch64Inst inst) {
    loadPairGeneric<uint64_t, LoadExclusive | LoadAcquire>(core, inst);
}

template <typename T, unsigned flags = 0>
void storeRegOffGeneric(Core* core, AArch64Inst inst) {
    bool offset64 = testBit<13>(inst.bits); // 10 - W reg, 11 - X reg
    bool offsetSigned = testBit<15>(inst.bits);
    constexpr uint64_t numBits = constLog2(sizeof(T));

    bool S = testBit<12>(inst.bits);
    uint64_t shift = (S)? numBits : 0;

    uint64_t imm = core->GPRegs[inst.Rm];
    if (!offset64) {
        imm &= bitmask<32, uint64_t>();
        if (offsetSigned) {
            imm = signExtend<32>(imm);
        }
    }
    imm <<= shift;

    uint64_t addr;
    if (inst.Rn == 31) {
        // TODO check SP alignment
        addr = core->SP;
    } else {
        addr = core->GPRegs[inst.Rn];
    }
    
    addr += imm;

    uint64_t data = 0;
    if constexpr (flags & StoreFloat) {
        data = core->VRegs[inst.Rt].D[0] & bitmask<(1 << numBits) * 8, uint64_t>();
    } else {
        data = core->GPRegs[inst.Rt] & bitmask<(1 << numBits) * 8, uint64_t>();
    }

    mem::write(addr, static_cast<T>(data));

    if constexpr (flags & Store128BitFloat) {
        data = core->VRegs[inst.Rt].D[1] & bitmask<(1 << numBits) * 8, uint64_t>();
        mem::write(addr + 8, static_cast<T>(data));
    }
}

void strbRegoff(Core* core, AArch64Inst inst) {
    storeRegOffGeneric<uint8_t>(core, inst);
}

void strhRegoff(Core* core, AArch64Inst inst) {
    storeRegOffGeneric<uint16_t>(core, inst);
}

void str32Regoff(Core* core, AArch64Inst inst) {
    storeRegOffGeneric<uint32_t>(core, inst);
}

void str64Regoff(Core* core, AArch64Inst inst) {
    storeRegOffGeneric<uint64_t>(core, inst);
}

void strbRegoffFloat(Core* core, AArch64Inst inst) {
    storeRegOffGeneric<uint8_t, StoreFloat>(core, inst);
}

void strhRegoffFloat(Core* core, AArch64Inst inst) {
    storeRegOffGeneric<uint16_t, StoreFloat>(core, inst);
}

void str32RegoffFloat(Core* core, AArch64Inst inst) {
    storeRegOffGeneric<uint32_t, StoreFloat>(core, inst);
}

void str64RegoffFloat(Core* core, AArch64Inst inst) {
    storeRegOffGeneric<uint64_t, StoreFloat>(core, inst);
}

void str128RegoffFloat(Core* core, AArch64Inst inst) {
    storeRegOffGeneric<uint64_t, StoreFloat | Store128BitFloat>(core, inst);
}

template <typename T, unsigned flags = 0>
void loadRegOffGeneric(Core* core, AArch64Inst inst) {
    bool offset64 = testBit<13>(inst.bits); // 10 - W reg, 11 - X reg
    bool offsetSigned = testBit<15>(inst.bits);
    constexpr uint64_t numBits = constLog2(sizeof(T));

    bool S = testBit<12>(inst.bits);
    uint64_t shift = (S)? numBits : 0;

    uint64_t imm = core->GPRegs[inst.Rm];
    if (!offset64) {
        imm &= bitmask<32, uint64_t>();
        if (offsetSigned) {
            imm = signExtend<32>(imm);
        }
    }
    imm <<= shift;

    uint64_t addr;
    if (inst.Rn == 31) {
        // TODO check SP alignment
        addr = core->SP;
    } else {
        addr = core->GPRegs[inst.Rn];
    }

    addr += imm;

    uint64_t data = mem::read<T>(addr);

    if constexpr (flags & LoadExtendResult) {
        data = signExtend<(1 << numBits) * 8>(data);
        if constexpr (!(flags & LoadExtendTo64)) {
            data &= bitmask<32, uint64_t>();
        }
    }

    if constexpr (flags & LoadFloat) {
        core->VRegs[inst.Rt].D[0] = data;
        if constexpr (flags & Load128BitFloat) {
            core->VRegs[inst.Rt].D[1] = mem::read<T>(addr + 8);
        } else {
            core->VRegs[inst.Rt].D[1] = 0;
        }
    } else {
        core->GPRegs[inst.Rt] = data;
    }
}

void ldrbRegoff(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint8_t>(core, inst);
}

void ldrsb32Regoff(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint8_t, LoadExtendResult>(core, inst);
}

void ldrsb64Regoff(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint8_t, LoadExtendResult | LoadExtendTo64>(core, inst);
}

void ldrhRegoff(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint16_t>(core, inst);
}

void ldrsh32Regoff(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint16_t, LoadExtendResult>(core, inst);
}

void ldrsh64Regoff(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint16_t, LoadExtendResult | LoadExtendTo64>(core, inst);
}

void ldr32Regoff(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint32_t>(core, inst);
}

void ldrswRegoff(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint32_t, LoadExtendResult | LoadExtendTo64>(core, inst);
}

void ldr64Regoff(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint64_t>(core, inst);
}

void ldrbRegoffFloat(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint8_t, LoadFloat>(core, inst);
}

void ldrhRegoffFloat(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint16_t, LoadFloat>(core, inst);
}

void ldr32RegoffFloat(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint32_t, LoadFloat>(core, inst);
}

void ldr64RegoffFloat(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint64_t, LoadFloat>(core, inst);
}

void ldr128RegoffFloat(Core* core, AArch64Inst inst) {
    loadRegOffGeneric<uint64_t, LoadFloat | Load128BitFloat>(core, inst);
}

template<typename T, unsigned flags = 0>
void loadRegLiteralGeneric(Core* core, AArch64Inst inst) {
    uint64_t off = signExtend<21>(static_cast<uint64_t>(inst._5_23 << 2));
    uint64_t addr = core->PC + off;
    uint64_t data = mem::read<T>(addr);

    if constexpr (flags & LoadExtendResult) {
        data = signExtend<32>(data);
    }

    if constexpr (flags & LoadFloat) {
        core->VRegs[inst.Rt].D[0] = data;

        if constexpr (flags & Load128BitFloat) {
            data = mem::read<T>(addr + 8);
            core->VRegs[inst.Rt].D[1] = data;
        }
    } else {
        core->GPRegs[inst.Rt] = data;
    }
}

void ldr32Literal(Core* core, AArch64Inst inst) {
    loadRegLiteralGeneric<uint32_t>(core, inst);
}

void ldr64Literal(Core* core, AArch64Inst inst) {
    loadRegLiteralGeneric<uint64_t>(core, inst);
}

void ldrswLiteral(Core* core, AArch64Inst inst) {
    loadRegLiteralGeneric<uint32_t, LoadExtendResult>(core, inst);
}

void ldr32LiteralFloat(Core* core, AArch64Inst inst) {
    loadRegLiteralGeneric<uint32_t, LoadFloat>(core, inst);
}

void ldr64LiteralFloat(Core* core, AArch64Inst inst) {
    loadRegLiteralGeneric<uint64_t, LoadFloat>(core, inst);
}

void ldr128Literal(Core* core, AArch64Inst inst) {
    loadRegLiteralGeneric<uint64_t, LoadFloat | Load128BitFloat>(core, inst);
}

void registerLoadStoreInstructions() {
    // Load/store exclusive
    registerInstruction(instructionID::STXRB, stxrbImm);
    registerInstruction(instructionID::STLXRB, stlxrbImm);
    registerInstruction(instructionID::LDXRB, ldxrbImm);
    registerInstruction(instructionID::LDAXRB, ldaxrbImm);
    registerInstruction(instructionID::STLRB, stlrbImm);
    registerInstruction(instructionID::LDARB, ldarbImm);
    registerInstruction(instructionID::STXRH, stxrhImm);
    registerInstruction(instructionID::STLXRH, stlxrhImm);
    registerInstruction(instructionID::LDXRH, ldxrhImm);
    registerInstruction(instructionID::LDAXRH, ldaxrhImm);
    registerInstruction(instructionID::STLRH, stlrhImm);
    registerInstruction(instructionID::LDARH, ldarhImm);
    registerInstruction(instructionID::STXR32, stxr32Imm);
    registerInstruction(instructionID::STLXR32, stlxr32Imm);
    registerInstruction(instructionID::STXP32, stxp32);
    registerInstruction(instructionID::STLXP32, stlxp32);
    registerInstruction(instructionID::LDXR32, ldxr32Imm);
    registerInstruction(instructionID::LDAXR32, ldaxr32Imm);
    registerInstruction(instructionID::LDXP32, ldxp32);
    registerInstruction(instructionID::LDAXP32, ldaxp32);
    registerInstruction(instructionID::STLR32, stlr32Imm);
    registerInstruction(instructionID::LDAR32, ldar32Imm);
    registerInstruction(instructionID::STXR64, stxr64Imm);
    registerInstruction(instructionID::STLXR64, stlxr64Imm);
    registerInstruction(instructionID::STXP64, stxp64);
    registerInstruction(instructionID::STLXP64, stlxp64);
    registerInstruction(instructionID::LDXR64, ldxr64Imm);
    registerInstruction(instructionID::LDAXR64, ldaxr64Imm);
    registerInstruction(instructionID::LDXP64, ldxp64);
    registerInstruction(instructionID::LDAXP64, ldaxp64);
    registerInstruction(instructionID::STLR64, stlr64Imm);
    registerInstruction(instructionID::LDAR64, ldar64Imm);
    // Load register (literal)
    registerInstruction(instructionID::LDR32_literal, ldr32Literal);
    registerInstruction(instructionID::LDR32_literal_float, ldr32LiteralFloat);
    registerInstruction(instructionID::LDR64_literal, ldr64Literal);
    registerInstruction(instructionID::LDR64_literal_float, ldr64LiteralFloat);
    registerInstruction(instructionID::LDRSW_literal, ldrswLiteral);
    registerInstruction(instructionID::LDR128_literal, ldr128Literal);
    registerInstruction(instructionID::PRFM_literal, nop);
    // Load/store no-allocate pair (offset)
    registerInstruction(instructionID::STNP32, stnp32);
    registerInstruction(instructionID::LDNP32, ldnp32);
    registerInstruction(instructionID::STNP32_float, stnp32Float);
    registerInstruction(instructionID::LDNP32_float, ldnp32Float);
    registerInstruction(instructionID::STNP64, stnp64);
    registerInstruction(instructionID::LDNP64, ldnp64);
    registerInstruction(instructionID::STNP64_float, stnp64Float);
    registerInstruction(instructionID::LDNP64_float, ldnp64Float);
    registerInstruction(instructionID::STNP128, stnp128Float);
    registerInstruction(instructionID::LDNP128, ldnp128Float);
    // Load/store register pair (post-indexed)
    registerInstruction(instructionID::STP32_post, stp32Post);
    registerInstruction(instructionID::LDP32_post, ldp32Post);
    registerInstruction(instructionID::STP32_post_float, stp32PostFloat);
    registerInstruction(instructionID::LDP32_post_float, ldp32PostFloat);
    registerInstruction(instructionID::LDPSW_post, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::STP64_post, stp64Post);
    registerInstruction(instructionID::LDP64_post, ldp64Post);
    registerInstruction(instructionID::STP64_post_float, stp64PostFloat);
    registerInstruction(instructionID::LDP64_post_float, ldp64PostFloat);
    registerInstruction(instructionID::STP128_post, stp128PostFloat);
    registerInstruction(instructionID::LDP128_post, ldp128PostFloat);
    // Load/store register pair (offset)
    registerInstruction(instructionID::STP32_offset, stp32Off);
    registerInstruction(instructionID::LDP32_offset, ldp32Off);
    registerInstruction(instructionID::STP32_offset_float, stp32OffFloat);
    registerInstruction(instructionID::LDP32_offset_float, ldp32OffFloat);
    registerInstruction(instructionID::LDPSW_offset, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::STP64_offset, stp64Off);
    registerInstruction(instructionID::LDP64_offset, ldp64Off);
    registerInstruction(instructionID::STP64_offset_float, stp64OffFloat);
    registerInstruction(instructionID::LDP64_offset_float, ldp64OffFloat);
    registerInstruction(instructionID::STP128_offset, stp128OffFloat);
    registerInstruction(instructionID::LDP128_offset, ldp128OffFloat);
    // Load/store register pair (pre-indexed)
    registerInstruction(instructionID::STP32_pre, stp32Pre);
    registerInstruction(instructionID::LDP32_pre, ldp32Pre);
    registerInstruction(instructionID::STP32_pre_float, stp32PreFloat);
    registerInstruction(instructionID::LDP32_pre_float, ldp32PreFloat);
    registerInstruction(instructionID::LDPSW_pre, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::STP64_pre, stp64Pre);
    registerInstruction(instructionID::LDP64_pre, ldp64Pre);
    registerInstruction(instructionID::STP64_pre_float, stp64PreFloat);
    registerInstruction(instructionID::LDP64_pre_float, ldp64PreFloat);
    registerInstruction(instructionID::STP128_pre, stp128PreFloat);
    registerInstruction(instructionID::LDP128_pre, ldp128PreFloat);
    // Load/store register (unscaled immediate)
    registerInstruction(instructionID::STURB, sturb);
    registerInstruction(instructionID::LDURB, ldurb);
    registerInstruction(instructionID::LDURSB64, ldursb64);
    registerInstruction(instructionID::LDURSB32, ldursb32);
    registerInstruction(instructionID::STUR8_float, stur8Float);
    registerInstruction(instructionID::LDUR8_float, ldur8Float);
    registerInstruction(instructionID::STUR128, stur128);
    registerInstruction(instructionID::LDUR128, ldur128);
    registerInstruction(instructionID::STURH, sturh);
    registerInstruction(instructionID::LDURH, ldurh);
    registerInstruction(instructionID::LDURSH64, ldursh64);
    registerInstruction(instructionID::LDURSH32, ldursh32);
    registerInstruction(instructionID::STUR16_float, stur16Float);
    registerInstruction(instructionID::LDUR16_float, ldur16Float);
    registerInstruction(instructionID::STUR32, stur32);
    registerInstruction(instructionID::LDUR32, ldur32);
    registerInstruction(instructionID::LDURSW, ldursw);
    registerInstruction(instructionID::STUR32_float, stur32Float);
    registerInstruction(instructionID::LDUR32_float, ldur32Float);
    registerInstruction(instructionID::STUR64, stur64);
    registerInstruction(instructionID::LDUR64, ldur64);
    registerInstruction(instructionID::PRFM_unscaled, nop);
    registerInstruction(instructionID::STUR64_float, stur64Float);
    registerInstruction(instructionID::LDUR64_float, ldur64Float);
    // Load/store register (immediate post-indexed)
    registerInstruction(instructionID::STRB_post, strbPost);
    registerInstruction(instructionID::LDRB_post, ldrbPost);
    registerInstruction(instructionID::LDRSB64_post, ldrsb64Post);
    registerInstruction(instructionID::LDRSB32_post, ldrsb32Post);
    registerInstruction(instructionID::STR8_post_float, str8PostFloat);
    registerInstruction(instructionID::LDR8_post_float, ldr8PostFloat);
    registerInstruction(instructionID::STR128_post, str128Post);
    registerInstruction(instructionID::LDR128_post, ldr128Post);
    registerInstruction(instructionID::STRH_post, strhPost);
    registerInstruction(instructionID::LDRH_post, ldrhPost);
    registerInstruction(instructionID::LDRSH64_post, ldrsh64Post);
    registerInstruction(instructionID::LDRSH32_post, ldrsh32Post);
    registerInstruction(instructionID::STR16_post_float, str16PostFloat);
    registerInstruction(instructionID::LDR16_post_float, ldr16PostFloat);
    registerInstruction(instructionID::STR32_post, str32Post);
    registerInstruction(instructionID::LDR32_post, ldr32Post);
    registerInstruction(instructionID::LDRSW_post, ldrswPost);
    registerInstruction(instructionID::STR32_post_float, str32PostFloat);
    registerInstruction(instructionID::LDR32_post_float, ldr32PostFloat);
    registerInstruction(instructionID::STR64_post, str64Post);
    registerInstruction(instructionID::LDR64_post, ldr64Post);
    registerInstruction(instructionID::STR64_post_float, str64PostFloat);
    registerInstruction(instructionID::LDR64_post_float, ldr64PostFloat);
    // Load/store register (unprivileged)
    registerInstruction(instructionID::STTRB, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::LDTRB, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::LDTRSB64, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::LDTRSB32, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::STTRH, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::LDTRH, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::LDTRSH64, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::LDTRSH32, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::STTR32, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::LDTR32, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::LDTRSW, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::STTR64, UNIMPLEMENTED_FUNC);
    registerInstruction(instructionID::LDTR64, UNIMPLEMENTED_FUNC);
    // Load/store register (immediate pre-indexed)
    registerInstruction(instructionID::STRB_pre, strbPre);
    registerInstruction(instructionID::LDRB_pre, ldrbPre);
    registerInstruction(instructionID::LDRSB64_pre, ldrsb64Pre);
    registerInstruction(instructionID::LDRSB32_pre, ldrsb32Pre);
    registerInstruction(instructionID::STR8_pre_float, str8PreFloat);
    registerInstruction(instructionID::LDR8_pre_float, ldr8PreFloat);
    registerInstruction(instructionID::STR128_pre, str128Pre);
    registerInstruction(instructionID::LDR128_pre, ldr128Pre);
    registerInstruction(instructionID::STRH_pre, strhPre);
    registerInstruction(instructionID::LDRH_pre, ldrhPre);
    registerInstruction(instructionID::LDRSH64_pre, ldrsh64Pre);
    registerInstruction(instructionID::LDRSH32_pre, ldrsh32Pre);
    registerInstruction(instructionID::STR16_pre_float, str16PreFloat);
    registerInstruction(instructionID::LDR16_pre_float, ldr16PreFloat);
    registerInstruction(instructionID::STR32_pre, str32Pre);
    registerInstruction(instructionID::LDR32_pre, ldr32Pre);
    registerInstruction(instructionID::LDRSW_pre, ldrswPre);
    registerInstruction(instructionID::STR32_pre_float, str32PreFloat);
    registerInstruction(instructionID::LDR32_pre_float, ldr32PreFloat);
    registerInstruction(instructionID::STR64_pre, str64Pre);
    registerInstruction(instructionID::LDR64_pre, ldr64Pre);
    registerInstruction(instructionID::STR64_pre_float, str64PreFloat);
    registerInstruction(instructionID::LDR64_pre_float, ldr64PreFloat);
    // Load/store register (register offset)
    registerInstruction(instructionID::STRB_reg, strbRegoff);
    registerInstruction(instructionID::LDRB_reg, ldrbRegoff);
    registerInstruction(instructionID::LDRSB64_reg, ldrsb64Regoff);
    registerInstruction(instructionID::LDRSB32_reg, ldrsb32Regoff);
    registerInstruction(instructionID::STR8_reg_float, strbRegoffFloat);
    registerInstruction(instructionID::LDR8_reg_float, ldrbRegoffFloat);
    registerInstruction(instructionID::STR128_reg, str128RegoffFloat);
    registerInstruction(instructionID::LDR128_reg, ldr128RegoffFloat);
    registerInstruction(instructionID::STRH_reg, strhRegoff);
    registerInstruction(instructionID::LDRH_reg, ldrhRegoff);
    registerInstruction(instructionID::LDRSH64_reg, ldrsh64Regoff);
    registerInstruction(instructionID::LDRSH32_reg, ldrsh32Regoff);
    registerInstruction(instructionID::STR16_reg_float, strhRegoffFloat);
    registerInstruction(instructionID::LDR16_reg_float, ldrhRegoffFloat);
    registerInstruction(instructionID::STR32_reg, str32Regoff);
    registerInstruction(instructionID::LDR32_reg, ldr32Regoff);
    registerInstruction(instructionID::LDRSW_reg, ldrswRegoff);
    registerInstruction(instructionID::STR32_reg_float, str32RegoffFloat);
    registerInstruction(instructionID::LDR32_reg_float, ldr32RegoffFloat);
    registerInstruction(instructionID::STR64_reg, str64Regoff);
    registerInstruction(instructionID::LDR64_reg, ldr64Regoff);
    registerInstruction(instructionID::PRFM_reg, nop);
    registerInstruction(instructionID::STR64_reg_float, str64RegoffFloat);
    registerInstruction(instructionID::LDR64_reg_float, ldr64RegoffFloat);
    // Load/store register (unsigned immediate)
    registerInstruction(instructionID::STRB_imm, strbImm);
    registerInstruction(instructionID::LDRB_imm, ldrbImm);
    registerInstruction(instructionID::LDRSB64_imm, ldrsb64Imm);
    registerInstruction(instructionID::LDRSB32_imm, ldrsb32Imm);
    registerInstruction(instructionID::STR8_imm_float, str8ImmFloat);
    registerInstruction(instructionID::LDR8_imm_float, ldr8ImmFloat);
    registerInstruction(instructionID::STR128_imm, str128Imm);
    registerInstruction(instructionID::LDR128_imm, ldr128Imm);
    registerInstruction(instructionID::STRH_imm, strhImm);
    registerInstruction(instructionID::LDRH_imm, ldrhImm);
    registerInstruction(instructionID::LDRSH64_imm, ldrsh64Imm);
    registerInstruction(instructionID::LDRSH32_imm, ldrsh32Imm);
    registerInstruction(instructionID::STR16_imm_float, str16ImmFloat);
    registerInstruction(instructionID::LDR16_imm_float, ldr16ImmFloat);
    registerInstruction(instructionID::STR32_imm, str32Imm);
    registerInstruction(instructionID::LDR32_imm, ldr32Imm);
    registerInstruction(instructionID::LDRSW_imm, ldrswImm);
    registerInstruction(instructionID::STR32_imm_float, str32ImmFloat);
    registerInstruction(instructionID::LDR32_imm_float, ldr32ImmFloat);
    registerInstruction(instructionID::STR64_imm, str64Imm);
    registerInstruction(instructionID::LDR64_imm, ldr64Imm);
    registerInstruction(instructionID::PRFM_imm, nop);
    registerInstruction(instructionID::STR64_imm_float, str64ImmFloat);
    registerInstruction(instructionID::LDR64_imm_float, ldr64ImmFloat);
}

} // namespace interpreter

} // namespace cpu