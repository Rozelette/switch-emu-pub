#pragma once
#include <array>

#pragma warning(push, 0)   
#include "asmjit\src\asmjit\asmjit.h"
#pragma warning(pop)

#include "cpu.h"
#include "emu_assert.h"
#include "log.h"
#include "unreachable.h"
#include "instruction.h"

// TODOs:
// Everything float
// Look-ahead
// SP in it's own register
// lockReg func
// Jump to middle of code blocks

namespace cpu::jit {

class AArch64EmuAssembler : public asmjit::X86Assembler {
public:
    AArch64EmuAssembler(asmjit::CodeHolder* code = nullptr) : asmjit::X86Assembler(code) {
        corePCMem = asmjit::x86::qword_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, PC)));
        coreSPMem = asmjit::x86::qword_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, SP)));
        coreNextPCMem = asmjit::x86::qword_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, nextPC)));

        for (int i = 0; i < 32; i++) {
            coreGPRegs[i] = asmjit::x86::qword_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, GPRegs[i])));
        }

        for (int i = 0; i < 32; i++) {
            coreVRegs[i][0] = asmjit::x86::qword_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, VRegs[i].D[0])));
            coreVRegs[i][1] = asmjit::x86::qword_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, VRegs[i].D[1])));
        }

        coreNflagMem = asmjit::x86::byte_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, PSTATE.N)));
        coreZflagMem = asmjit::x86::byte_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, PSTATE.Z)));
        coreCflagMem = asmjit::x86::byte_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, PSTATE.C)));
        coreVflagMem = asmjit::x86::byte_ptr(stateReg, static_cast<int32_t>(offsetof(cpu::Core, PSTATE.V)));

        // TODO can we put more regs in here? rax?
        mGpRegs[0] = asmjit::x86::rbx;
        mGpRegs[1] = asmjit::x86::rdi;
        mGpRegs[2] = asmjit::x86::rsi;
        mGpRegs[3] = asmjit::x86::rbp;
        mGpRegs[4] = asmjit::x86::r10;
        mGpRegs[5] = asmjit::x86::r11;
        mGpRegs[6] = asmjit::x86::r12;
        mGpRegs[7] = asmjit::x86::r13;
        mGpRegs[8] = asmjit::x86::r14;

        for (auto i = 0u; i < 9; i++) {
            mRegs[i] = HostReg(RegType::GP, i);
        }
    }

    uint64_t currentPC;

    asmjit::X86Gp stateReg = asmjit::x86::r15;

    std::array<asmjit::X86Mem, 32> coreGPRegs;
    std::array<std::array<asmjit::X86Mem, 2>, 32> coreVRegs; // TODO is this the right struct for this?

    asmjit::X86Mem coreNflagMem;
    asmjit::X86Mem coreZflagMem;
    asmjit::X86Mem coreCflagMem;
    asmjit::X86Mem coreVflagMem;

    asmjit::X86Mem corePCMem;
    asmjit::X86Mem coreSPMem;
    asmjit::X86Mem coreNextPCMem;

    uint64_t lruCounter = 0;

    // TODO assumes windows
    std::array<asmjit::X86Gp, 4> argRegs = {
        asmjit::x86::rcx,
        asmjit::x86::rdx,
        asmjit::x86::r8,
        asmjit::x86::r9
    };

    std::array<asmjit::X86Gp, 9> mGpRegs;

    enum class RegType {
        NONE = 0,
        GP
    };

    struct HostReg {
        HostReg() {}
        HostReg(RegType type, uint32_t id) : mType(type), mRegID(id) {}

        RegType mType = RegType::NONE;
        uint32_t mRegID = 0;

        static constexpr uint64_t NO_LRU_VALUE = 0xFFFFFFFFFFFFFFFF;
        uint32_t useCount = 0;
        uint64_t lru = NO_LRU_VALUE;

        static constexpr int32_t NO_OFFSET = 0x7FFFFFFF;
        int32_t mOffset = NO_OFFSET;

        bool mModified = false;
        bool mLoaded = false;
    };

    std::array<HostReg, 9> mRegs;

    HostReg* allocReg(RegType type) {
        uint32_t lruReg = 0xFFFFFFFF;
        uint64_t lruValue = HostReg::NO_LRU_VALUE;
        for (auto i = 0u; i < 9; i++) {
            auto& reg = mRegs[i];
            if (reg.mType != type) {
                continue;
            }

            if (reg.useCount == 0) {
                if (reg.mOffset == HostReg::NO_OFFSET) {
                    reg.useCount++;

                    //logger::info("allocReg: return id %d", i);

                    return &reg;
                }

                if (reg.lru != HostReg::NO_LRU_VALUE) {
                    if (reg.lru < lruValue) {
                        lruValue = reg.lru;
                        lruReg = i;
                    }
                }
            }
        }

        if (lruReg != 0xFFFFFFFF) {
            auto& reg = mRegs[lruReg];
            envict(&reg);
            reg.useCount++;
            return &reg;
        }

        emu_abort("Could not allocate register for JIT");
    }

    void envict(HostReg* reg) {
        //emu_assert(reg->lru != HostReg::NO_LRU_VALUE, "Tried to envict permanent register");
        emu_assert(reg->useCount == 0, "Tried to envict register that is in use");
        emu_assert(reg->mOffset != HostReg::NO_OFFSET, "Tried to envict register that did not have an associated offset");
        emu_assert(reg->mType == RegType::GP, "Only GP regs for now");

        //logger::info("Envicting reg: %d", reg->mRegID);

        if (reg->mModified) {
            emu_assert(reg->mLoaded, "Tried to write a register that was not loaded");
            mov(asmjit::X86Mem(stateReg, reg->mOffset, 8), mGpRegs[reg->mRegID]);
        }

        reg->lru = HostReg::NO_LRU_VALUE;
        reg->mOffset = HostReg::NO_OFFSET;
        reg->useCount = 0;
        reg->mModified = false;
        reg->mLoaded = false;
    }

    void envictAll() {
        for (auto& reg : mRegs) {
            if (reg.mOffset != HostReg::NO_OFFSET) {
                envict(&reg);
            }
        }
    }

    class Register {
        friend AArch64EmuAssembler;

        Register(AArch64EmuAssembler* parent, HostReg* reg, uint32_t size, bool writtenOnUse) : mParent(parent), mReg(reg), mSize(size), mWrittenOnUse(writtenOnUse){}

        Register(const Register& other) {
            if (other.mReg) {
                other.mReg->useCount++;
            }

            mParent = other.mParent;
            mReg = other.mReg;
            mSize = other.mSize;
            mWrittenOnUse = other.mWrittenOnUse;
        }

        Register& operator=(const Register& other) {
            if (mReg) {
                mReg->useCount--;
            }

            if (other.mReg) {
                other.mReg->useCount++;
            }

            mParent = other.mParent;
            mReg = other.mReg;
            mSize = other.mSize;
            mWrittenOnUse = other.mWrittenOnUse;

            return *this;
        }

        void markWritten() const {
            mReg->lru = mParent->lruCounter++;
            if (mWrittenOnUse) {
                mReg->mModified = true;
                mReg->mLoaded = true;
            }
        }

        ~Register() {
            if (mReg) {
                mReg->useCount--;
            }
        }

        AArch64EmuAssembler* mParent;
        HostReg* mReg;
        uint32_t mSize;
        bool mWrittenOnUse;
    };

    class GpRegister : public Register {
        friend AArch64EmuAssembler;
    public:
        GpRegister(AArch64EmuAssembler* parent, HostReg* reg, uint32_t size, bool writtenOnUse) : Register(parent, reg, size, writtenOnUse) {}

        GpRegister(const GpRegister &other) : Register(other) {}

        GpRegister & operator=(const GpRegister &other) {
            Register::operator=(other);
            return *this;
        }

        GpRegister r8() {
            mReg->useCount++;
            return GpRegister(mParent, mReg, 1, mWrittenOnUse);
        }

        GpRegister r16() {
            mReg->useCount++;
            return GpRegister(mParent, mReg, 2, mWrittenOnUse);
        }

        GpRegister r32() {
            mReg->useCount++;
            return GpRegister(mParent, mReg, 4, mWrittenOnUse);
        }

        GpRegister r64() {
            mReg->useCount++;
            return GpRegister(mParent, mReg, 8, mWrittenOnUse);
        }

        operator asmjit::X86Gp() const {
            markWritten();
            if (mSize == 1) {
                return mParent->mGpRegs[mReg->mRegID].r8();
            } else if (mSize == 2) {
                return mParent->mGpRegs[mReg->mRegID].r16();
            } else if (mSize == 4) {
                return mParent->mGpRegs[mReg->mRegID].r32();
            } else {
                emu_assert(mSize == 8, "Illegal size in Register");
                return mParent->mGpRegs[mReg->mRegID].r64();
            }
        }
    };

    GpRegister allocGpTemp(uint32_t size) {
        return GpRegister(this, allocReg(RegType::GP), size, false);
    }

    GpRegister allocGpTemp(GpRegister& other) {
        auto reg = allocGpTemp(other.mSize);

        mov(reg, other);

        return reg;
    }

    GpRegister allocGpTemp(uint32_t size, uint64_t imm) {
        auto reg = allocGpTemp(size);

        mov(reg, imm);

        return reg;
    }

    HostReg* findReg(int32_t offset) {
        for (auto& reg : mRegs) {
            if (reg.mOffset == offset) {
                return &reg;
            }
        }

        return nullptr;
    }

    GpRegister loadGpRegister(asmjit::X86Mem ref, bool loadReg, bool markWrittenOnUse) {
        auto reg = findReg(ref.getOffsetLo32());
        if (reg && (reg->mType != RegType::GP)) {
            emu_abort("This should never happen?");
        }
        
        if (reg) {
            reg->useCount++;
        } else {
            reg = allocReg(RegType::GP);
            reg->mOffset = ref.getOffsetLo32();
        }

        if (loadReg & !(reg->mLoaded)) {
            mov(mGpRegs[reg->mRegID], ref);
            reg->mLoaded = true;
        }

        reg->lru = lruCounter++;

        //logger::info("Loaded reg into: %d and offset: %08X", reg->mRegID, reg->mOffset);

        return GpRegister(this, reg, 8, markWrittenOnUse);
    }

    GpRegister loadGpRegisterRead(asmjit::X86Mem ref) {
        return loadGpRegister(ref, true, false);
    }

    GpRegister loadGpRegisterWrite(asmjit::X86Mem ref) {
        return loadGpRegister(ref, false, true);
    }

    GpRegister loadGpRegisterReadWrite(asmjit::X86Mem ref) {
        return loadGpRegister(ref, true, true);
    }

    // TODO maybe this can return a register that it allocates on demand?
    // or maybe have a label as an input that is jumps to for you?
    void insertTestCondition(Condition cond, asmjit::X86Gp reg) {
        switch (cond) {
        case Condition::EQ:
            mov(reg, coreZflagMem);
            break;
        case Condition::NE:
            mov(reg, coreZflagMem);
            test(reg, reg);
            sete(reg);
            break;
        case Condition::CS:
            mov(reg, coreCflagMem);
            break;
        case Condition::CC:
            mov(reg, coreCflagMem);
            test(reg, reg);
            sete(reg);
            break;
        case Condition::MI:
            mov(reg, coreNflagMem);
            break;
        case Condition::PL:
            mov(reg, coreNflagMem);
            test(reg, reg);
            sete(reg);
            break;
        case Condition::VS:
            mov(reg, coreVflagMem);
            break;
        case Condition::VC:
            mov(reg, coreVflagMem);
            test(reg, reg);
            sete(reg);
            break;
        case Condition::HI:
            mov(reg, coreZflagMem);
            test(reg, 0xFF);
            sete(reg);
            and_(reg, coreCflagMem);
            break;
        case Condition::LS:
            mov(reg, coreCflagMem);
            test(reg, 0xFF);
            sete(reg);
            or_(reg, coreZflagMem);
            break;
        case Condition::GE:
            mov(reg, coreVflagMem);
            cmp(reg, coreNflagMem);
            sete(reg);
            break;
        case Condition::LT:
            mov(reg, coreVflagMem);
            cmp(reg, coreNflagMem);
            sete(reg);
            test(reg, reg);
            sete(reg);
            break;
        case Condition::GT:
            mov(reg, coreVflagMem);
            cmp(reg, coreNflagMem);
            sete(reg);
            test(reg, reg);
            sete(reg);
            or_(reg, coreZflagMem);
            test(reg, reg);
            sete(reg);
            break;
        case Condition::LE:
            mov(reg, coreVflagMem);
            cmp(reg, coreNflagMem);
            sete(reg);
            test(reg, reg);
            sete(reg);
            or_(reg, coreZflagMem);
            break;
        case Condition::AL:
        case Condition::A2:
            mov(reg, 1);
            break;
        default:
            UNREACHABLE();
            break;
        }
    }
};

using JitCode = void(*)();
using jitfptr_t = void(*)(AArch64EmuAssembler&, AArch64Inst);
using JitEntryFunc = void(*)(Core*, JitCode);
using JitFinaleFunc = JitEntryFunc;

// TODO seperate header for internals
struct JitBlock {
    JitCode funcEntry;

    uint64_t start;
    uint64_t end;

    JitBlock(uint64_t start_) : start(start_), end(start_), funcEntry(nullptr) {}
};

extern JitEntryFunc gBlockEntry;
extern JitFinaleFunc gBlockFinale;

void init();
void registerInstruction(instructionID id, jitfptr_t func);
bool generate(JitBlock& block);

// TODO better name
void step(Core* core);
void executeCode(Core* core, JitCode func);

// TODO jit helper header
void FALLBACK(AArch64EmuAssembler& a, AArch64Inst inst);

inline void nop(AArch64EmuAssembler&, AArch64Inst) {}

void write64ImmToMem(AArch64EmuAssembler& a, asmjit::X86Mem& mem, uint64_t data);

} // namespace cpu::jit