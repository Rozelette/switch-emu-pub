#pragma once
#include <iostream>
#include "util.h"

#include "cpu.h"
#include "interpreter\interpreter.h"
#include "mem.h"
#include "jit\jit.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterpreterUnitTest {
template<cpu::instructionID testInstId>
class BaseTestClass {
public:
    cpu::Core core;
    cpu::Core expected;
    cpu::Core jitCore;

    cpu::interpreter::interptfptr_t instFunc;
    cpu::AArch64Inst inst;

    static const size_t ERR_BUFFER_SIZE = 512;
    static const uint64_t NUM_PAGES = 0x10;

    static const uint64_t GPREG_DEFAULT = 0;
    static const uint64_t VREG_DEFAULT0 = 0;
    static const uint64_t VREG_DEFAULT1 = 0;
    static const uint64_t SP_DEFAULT = 0x8070605040302010;
    static const uint64_t PC_DEFAULT = 0x0000000076543210;
    static const uint64_t NEXT_PC_DEFAULT = PC_DEFAULT + 4;
    static const uint64_t TLS_DEFAULT = 0x1234123412340000;

    void init() {
        mem::init(mem::PAGE_SIZE * NUM_PAGES);

        coreInitializer(core);
        coreInitializer(expected);
        coreInitializer(jitCore);
        instFunc = cpu::interpreter::getInstructionFunc(testInstId);
        inst.bits = cpu::encoder::encode(testInstId);

        cpu::jit::init();

        cpu::setInterruptHandler(static_cast<cpu::InterruptHandler>(&BaseTestClass::interruptHandler));
        cpu::setSegFaultHandler(static_cast<cpu::SegFaultHandler>(&BaseTestClass::segFaultHandler));
        cpu::setIllInstHandler(static_cast<cpu::IllInstHandler>(&BaseTestClass::illInstHandler));
    }

    void fini() {
        mem::fini();
    }

    static void interruptHandler(uint64_t interrupts) {
        Assert::Fail(L"Unexpected interrupt");
    }

    static void segFaultHandler(uint64_t addr) {
        Assert::Fail(L"Seg Fault");
    }

    static void illInstHandler() {
        Assert::Fail(L"Illegal instruction");
    }

    // TODO more flags
    bool testFlags(const PSTATE_t& expected, const PSTATE_t& actual, std::wstringstream& err) {
        bool ret = true;
        if (expected.N != actual.N) {
            err << L"\nUnexpected value in N: expected " << expected.N << L" got "<< actual.N;
            ret = false;
        }
        if (expected.Z != actual.Z) {
            err << L"\nUnexpected value in Z: expected " << expected.Z << L" got " << actual.Z;
            ret = false;
        }
        if (expected.C != actual.C) {
            err << L"\nUnexpected value in C: expected " << expected.C << L" got " << actual.C;
            ret = false;
        }
        if (expected.V != actual.V) {
            err << L"\nUnexpected value in V: expected " << expected.V << L" got " << actual.V;
            ret = false;
        }

        return ret;
    }

    bool compareCores(const cpu::Core& expected, const cpu::Core& actual, std::wstringstream& err) {
        bool ret = true;

        for (auto i = 0; i < 31; i++) {
            if (expected.GPRegs[i] != actual.GPRegs[i]) {
                err << L"\nUnexpected value in X" << i << L": expected 0x";
                err.width(16);
                err.fill('0');
                err << std::hex << expected.GPRegs[i];

                err << L" got 0x";

                err.width(16);
                err.fill('0');
                err << std::hex << actual.GPRegs[i];

                ret = false;
            }
        }

        for (auto i = 0; i < 32; i++) {
            if ((expected.VRegs[i].D[0] != actual.VRegs[i].D[0]) || (expected.VRegs[i].D[1] != actual.VRegs[i].D[1])) {
                err << L"\nUnexpected value in V" << i << L": expected 0x";
                err.width(16);
                err.fill('0');
                err << std::hex << expected.VRegs[i].D[1];
                err.width(16);
                err.fill('0');
                err << std::hex << expected.VRegs[i].D[0];

                err << L" got 0x";

                err.width(16);
                err.fill('0');
                err << std::hex << actual.VRegs[i].D[1];
                err.width(16);
                err.fill('0');
                err << std::hex << actual.VRegs[i].D[0];
                ret = false;
            }
        }

        if (expected.PC != actual.PC) {
            err << L"\nUnexpected PC: expected 0x";
            err.width(16);
            err.fill('0');
            err << std::hex << expected.PC;

            err << L" got 0x";

            err.width(16);
            err.fill('0');
            err << std::hex << actual.PC;

            ret = false;
        }

        if (expected.SP != actual.SP) {
            err << L"\nUnexpected SP: expected 0x";
            err.width(16);
            err.fill('0');
            err << std::hex << expected.SP;

            err << L" got 0x";

            err.width(16);
            err.fill('0');
            err << std::hex << actual.SP;

            ret = false;
        }

        if (expected.TLSAddr != actual.TLSAddr) {
            err << L"\nUnexpected TLSAddr: expected 0x";
            err.width(16);
            err.fill('0');
            err << std::hex << expected.TLSAddr;

            err << L" got 0x";

            err.width(16);
            err.fill('0');
            err << std::hex << actual.TLSAddr;

            ret = false;
        }

        if (expected.nextPC != actual.nextPC) {
            err << L"\nUnexpected nextPC: expected 0x";
            err.width(16);
            err.fill('0');
            err << std::hex << expected.nextPC;

            err << L" got 0x";

            err.width(16);
            err.fill('0');
            err << std::hex << actual.nextPC;

            ret = false;
        }

        if (expected.exclusiveAccess != actual.exclusiveAccess) {
            err << L"\nUnexpected exclusiveAccess: expected " << expected.exclusiveAccess << L" got " << actual.exclusiveAccess;

            ret = false;
        }

        if (expected.exclusiveAccess && actual.exclusiveAccess) {
            if (expected.exclusiveAccessAddr != actual.exclusiveAccessAddr) {
                err << L"\nUnexpected exclusiveAccessAddr: expected 0x";
                err.width(16);
                err.fill('0');
                err << std::hex << expected.exclusiveAccessAddr;

                err << L" got 0x";

                err.width(16);
                err.fill('0');
                err << std::hex << actual.exclusiveAccessAddr;
                
                ret = false;
            }

            if ((expected.exclusiveData0 != actual.exclusiveData0) || (expected.exclusiveData1 != actual.exclusiveData1)) {
                err << L"\nUnexpected exclusiveData: expected 0x";
                err.width(16);
                err.fill('0');
                err << std::hex << expected.exclusiveData1;
                err.width(16);
                err.fill('0');
                err << std::hex << expected.exclusiveData0;

                err << L" got 0x";

                err.width(16);
                err.fill('0');
                err << std::hex << actual.exclusiveData1;
                err.width(16);
                err.fill('0');
                err << std::hex << actual.exclusiveData0;

                ret = false;
            }
        }

        ret &= testFlags(expected.PSTATE, actual.PSTATE, err);

        return ret;
    }

    void coreInitializer(cpu::Core& core) {
        for (int i = 0; i < 32; i++) {
            core.GPRegs[i] = GPREG_DEFAULT;
            core.VRegs[i].D[0] = VREG_DEFAULT0;
            core.VRegs[i].D[1] = VREG_DEFAULT1;
        }

        core.SP = SP_DEFAULT;
        core.PC = PC_DEFAULT;
        core.nextPC = NEXT_PC_DEFAULT;
        core.TLSAddr = TLS_DEFAULT;

        core.exclusiveAccess = false;
        core.exclusiveAccessAddr = 0;
        core.exclusiveData0 = 0;
        core.exclusiveData1 = 0;

        core.PSTATE.N = false;
        core.PSTATE.Z = false;
        core.PSTATE.C = false;
        core.PSTATE.V = false;
    }

    void doTestInst(const wchar_t* msg) {
        // TODO try to relax or eliminate these constraints
        bool canCopyToMem = (core.PC < (1llu << 38));
        if (canCopyToMem) {
            Assert::IsTrue(mem::allocatePage(mem::getPageNum(core.PC)), L"Could not allocate page for instruction");
            mem::protectPage(mem::getPageNum(core.PC), mem::Permission::READ_EXECUTE);

            // TODO this randomly fails???
            mem::write(core.PC, inst);
        }

        instFunc(&core, inst);

        std::wstringstream errIter;
        errIter << L"[Interpreter] " << msg;
        bool same = compareCores(expected, core, errIter);
        Assert::IsTrue(same, errIter.str().c_str());

        if (canCopyToMem) {
            // TODO try to get the jit where it does not need mem to generate blocks
            cpu::jit::JitBlock block(core.PC);
            block.end = core.PC + 4;
            Assert::IsTrue(cpu::jit::generate(block), L"Could not generate jit function");
            cpu::jit::executeCode(&jitCore, block.funcEntry);

            std::wstringstream errJit;
            errJit << L"[Jit] " << msg;
            bool same = compareCores(expected, jitCore, errJit);
            Assert::IsTrue(same, errJit.str().c_str());
        }

        coreInitializer(core);
        coreInitializer(expected);
        coreInitializer(jitCore);
    }

    // TODO pretty print condition and case
    void testAllConditions(std::function<void()> setupIfMet, std::function<void()> setupIfNotMet) {
        wchar_t ERROR_MESSAGE[ERR_BUFFER_SIZE];
        for (uint64_t i = 0; i < 16; i++) {
            inst.cond = i;
            for (uint64_t j = 0; j < 16; j++) {
                SET_REG(PSTATE.N, (j & 0b0001) == 0b0001);
                SET_REG(PSTATE.Z, (j & 0b0010) == 0b0010);
                SET_REG(PSTATE.C, (j & 0b0100) == 0b0100);
                SET_REG(PSTATE.V, (j & 0b1000) == 0b1000);
                if (truthTable[i][j]) {
                    setupIfMet();
                } else {
                    setupIfNotMet();
                }
                swprintf_s(ERROR_MESSAGE, L"Test failed for condition %1llX with flags %1llX", i, j);
                doTestInst(ERROR_MESSAGE);
            }
        }
    }
};
}