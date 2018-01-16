#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterpreterUnitTest {

TEST_CLASS(udiv32), BaseTestClass<cpu::instructionID::UDIV_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(UDIV_32_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 1;
        doTestInst(L"UDIV_32 does basic division");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 does basic division");

        SET_REG(GPRegs[1], 2);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 2;
        doTestInst(L"UDIV_32 does basic division");

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 2);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 does basic division");

        SET_REG(GPRegs[1], 1000);
        SET_REG(GPRegs[2], 999);
        expected.GPRegs[3] = 1;
        doTestInst(L"UDIV_32 rounds towards 0");

        SET_REG(GPRegs[1], 1000);
        SET_REG(GPRegs[2], 1001);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 rounds towards 0");
    }

    TEST_METHOD(UDIV_32_div_by_0) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 0);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 results in 0 when dividing by 0");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 0);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 results in 0 when dividing by 0");
    }

    TEST_METHOD(UDIV_32_reg_size) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"UDIV_32 uses only lower 32 bits of reg");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000001);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"UDIV_32 uses only lower 32 bits of reg");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000000);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 uses only lower 32 bits of reg");

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        SET_REG(GPRegs[3], 0xFFFFFFFFFFFFFFFF);
        expected.GPRegs[3] = 1;
        doTestInst(L"UDIV_32 clears upper 32 bits of reg");
    }

    TEST_METHOD(UDIV_32_0_reg) {
        SET_REG(SP, 1);
        SET_REG(GPRegs[2], 1);
        inst.Rn = 31;
        inst.Rm = 2;
        inst.Rd = 3;
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 uses 0 reg");

        SET_REG(SP, 1);
        SET_REG(GPRegs[1], 1);
        inst.Rn = 1;
        inst.Rm = 31;
        inst.Rd = 3;
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 uses 0 reg");

        SET_REG(SP, 2);
        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 31;
        doTestInst(L"UDIV_32 uses 0 reg");
    }

    TEST_METHOD(UDIV_32_unsigned) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 0xFFFFFFFF);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 is an unsigned division");

        SET_REG(GPRegs[1], 0x7FFFFFFF);
        SET_REG(GPRegs[2], 0x80000001);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_32 is an unsigned division");
    }
};


TEST_CLASS(sdiv32), BaseTestClass<cpu::instructionID::SDIV_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(SDIV_32_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 1;
        doTestInst(L"SDIV_32 does basic division");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_32 does basic division");

        SET_REG(GPRegs[1], 2);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 2;
        doTestInst(L"SDIV_32 does basic division");

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 2);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_32 does basic division");

        SET_REG(GPRegs[1], 1000);
        SET_REG(GPRegs[2], 999);
        expected.GPRegs[3] = 1;
        doTestInst(L"SDIV_32 rounds towards 0");

        SET_REG(GPRegs[1], 1000);
        SET_REG(GPRegs[2], 1001);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_32 rounds towards 0");

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 0xFFFFF002);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"SDIV_32 rounds towards 0");

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 0xFFFFEFFF);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_32 rounds towards 0");
    }

    TEST_METHOD(SDIV_32_div_by_0) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 0);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_32 results in 0 when dividing by 0");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 0);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_32 results in 0 when dividing by 0");
    }

    TEST_METHOD(SDIV_32_reg_size) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"SDIV_32 uses only lower 32 bits of reg");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000001);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"SDIV_32 uses only lower 32 bits of reg");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000000);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_32 uses only lower 32 bits of reg");

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        SET_REG(GPRegs[3], 0xFFFFFFFFFFFFFFFF);
        expected.GPRegs[3] = 1;
        doTestInst(L"SDIV_32 clears upper 32 bits of reg");
    }

    TEST_METHOD(SDIV_32_0_reg) {
        SET_REG(SP, 1);
        SET_REG(GPRegs[2], 1);
        inst.Rn = 31;
        inst.Rm = 2;
        inst.Rd = 3;
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_32 uses 0 reg");

        SET_REG(SP, 1);
        SET_REG(GPRegs[1], 1);
        inst.Rn = 1;
        inst.Rm = 31;
        inst.Rd = 3;
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_32 uses 0 reg");

        SET_REG(SP, 2);
        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 31;
        doTestInst(L"SDIV_32 uses 0 reg");
    }

    TEST_METHOD(SDIV_32_signed) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 0xFFFFFFFF);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"SDIV_32 is a signed division");

        SET_REG(GPRegs[1], 0x7FFFFFFF);
        SET_REG(GPRegs[2], 0x80000001);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"SDIV_32 is a signed division");
    }

    TEST_METHOD(SDIV_32_overflow) {
        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0xFFFFFFFF);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;
        expected.GPRegs[3] = 0x80000000;
        doTestInst(L"SDIV_32 produces 0x80000000 on 0x80000000/0xFFFFFFFF");
    }
};

TEST_CLASS(udiv64), BaseTestClass<cpu::instructionID::UDIV_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(UDIV_64_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 1;
        doTestInst(L"UDIV_64 does basic division");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_64 does basic division");

        SET_REG(GPRegs[1], 2);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 2;
        doTestInst(L"UDIV_64 does basic division");

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 2);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_64 does basic division");

        SET_REG(GPRegs[1], 1000);
        SET_REG(GPRegs[2], 999);
        expected.GPRegs[3] = 1;
        doTestInst(L"UDIV_64 rounds towards 0");

        SET_REG(GPRegs[1], 1000);
        SET_REG(GPRegs[2], 1001);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_64 rounds towards 0");
    }

    TEST_METHOD(UDIV_64_div_by_0) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 0);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_64 results in 0 when dividing by 0");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 0);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_64 results in 0 when dividing by 0");
    }

    TEST_METHOD(UDIV_64_reg_size) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 0xFFFFFFFFFFFFFFFF;
        doTestInst(L"UDIV_64 uses all 64 bits");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000001);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"UDIV_64 uses all 64 bits");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000000);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"UDIV_64 uses all 64 bits");
    }

    TEST_METHOD(UDIV_64_0_reg) {
        SET_REG(SP, 1);
        SET_REG(GPRegs[2], 1);
        inst.Rn = 31;
        inst.Rm = 2;
        inst.Rd = 3;
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_64 uses 0 reg");

        SET_REG(SP, 1);
        SET_REG(GPRegs[1], 1);
        inst.Rn = 1;
        inst.Rm = 31;
        inst.Rd = 3;
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_64 uses 0 reg");

        SET_REG(SP, 2);
        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 31;
        doTestInst(L"UDIV_64 uses 0 reg");
    }

    TEST_METHOD(UDIV_64_unsigned) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 0xFFFFFFFFFFFFFFFF);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_64 is an unsigned division");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x8000000000000001);
        expected.GPRegs[3] = 0;
        doTestInst(L"UDIV_64 is an unsigned division");
    }
};

TEST_CLASS(sdiv64), BaseTestClass<cpu::instructionID::SDIV_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(SDIV_64_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 1;
        doTestInst(L"SDIV_64 does basic division");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 does basic division");

        SET_REG(GPRegs[1], 2);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 2;
        doTestInst(L"SDIV_64 does basic division");

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 2);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 does basic division");

        SET_REG(GPRegs[1], 1000);
        SET_REG(GPRegs[2], 999);
        expected.GPRegs[3] = 1;
        doTestInst(L"SDIV_64 rounds towards 0");

        SET_REG(GPRegs[1], 1000);
        SET_REG(GPRegs[2], 1001);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 rounds towards 0");

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 0xFFFFFFFFFFFFF002);
        expected.GPRegs[3] = 0xFFFFFFFFFFFFFFFF;
        doTestInst(L"SDIV_64 rounds towards 0");

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 0xFFFFFFFFFFFFEFFF);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 rounds towards 0");
    }

    TEST_METHOD(SDIV_64_div_by_0) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 0);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 results in 0 when dividing by 0");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 0);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 results in 0 when dividing by 0");
    }

    TEST_METHOD(SDIV_64_reg_size) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 1);
        expected.GPRegs[3] = 0xFFFFFFFFFFFFFFFF;
        doTestInst(L"SDIV_64 uses all 64 bits");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000001);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 uses all 64 bits");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000000);
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 uses all 64 bits");
    }

    TEST_METHOD(SDIV_64_0_reg) {
        SET_REG(SP, 1);
        SET_REG(GPRegs[2], 1);
        inst.Rn = 31;
        inst.Rm = 2;
        inst.Rd = 3;
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 uses 0 reg");

        SET_REG(SP, 1);
        SET_REG(GPRegs[1], 1);
        inst.Rn = 1;
        inst.Rm = 31;
        inst.Rd = 3;
        expected.GPRegs[3] = 0;
        doTestInst(L"SDIV_64 uses 0 reg");

        SET_REG(SP, 2);
        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 1);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 31;
        doTestInst(L"SDIV_64 uses 0 reg");
    }

    TEST_METHOD(SDIV_64_signed) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 1);
        SET_REG(GPRegs[2], 0xFFFFFFFFFFFFFFFF);
        expected.GPRegs[3] = 0xFFFFFFFFFFFFFFFF;
        doTestInst(L"SDIV_64 is a signed division");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x8000000000000001);
        expected.GPRegs[3] = 0xFFFFFFFFFFFFFFFF;
        doTestInst(L"SDIV_64 is a signed division");
    }

    TEST_METHOD(SDIV_64_overflow) {
        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0xFFFFFFFFFFFFFFFF);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;
        expected.GPRegs[3] = 0x8000000000000000;
        doTestInst(L"SDIV_64 produces 0x8000000000000000 on 0x8000000000000000/0xFFFFFFFFFFFFFFFF");
    }
};

TEST_CLASS(lslv32), BaseTestClass<cpu::instructionID::LSLV_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(LSLV_32_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x1;
        doTestInst(L"LSLV_32 does nothing when shift is 0");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x2;
        doTestInst(L"LSLV_32 can shift");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x1f);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x80000000;
        doTestInst(L"LSLV_32 can shift");
    }

    TEST_METHOD(LSLV_32_mod) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x21);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x2;
        doTestInst(L"LSLV_32 shift is mod 32");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x3456378ABC356721);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x2;
        doTestInst(L"LSLV_32 shift is mod 32");
    }

    TEST_METHOD(LSLV_32_reg_size) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFE;
        doTestInst(L"LSLV_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000001);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFE;
        doTestInst(L"LSLV_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"LSLV_32 uses 32bit registers");
    }

    TEST_METHOD(LSLV_32_same_reg) {

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x8;
        doTestInst(L"LSLV_32 can use the same register");

        SET_REG(GPRegs[1], 0x21);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x42;
        doTestInst(L"LSLV_32 can use the same register");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x4;
        doTestInst(L"LSLV_32 can use the same register");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 2;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x4;
        doTestInst(L"LSLV_32 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x8;
        doTestInst(L"LSLV_32 can use the same register");

        SET_REG(GPRegs[1], 0x21);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x42;
        doTestInst(L"LSLV_32 can use the same register");
    }
};

TEST_CLASS(lsrv32), BaseTestClass<cpu::instructionID::LSRV_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(LSRV_32_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x80000000;
        doTestInst(L"LSRV_32 does nothing when shift is 0");

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x40000000;
        doTestInst(L"LSRV_32 can shift");

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x1f);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x1;
        doTestInst(L"LSRV_32 can shift");
    }

    TEST_METHOD(LSRV_32_mod) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x21);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x40000000;
        doTestInst(L"LSRV_32 shift is mod 32");

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x3456378ABC356721);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x40000000;
        doTestInst(L"LSRV_32 shift is mod 32");
    }

    TEST_METHOD(LSRV_32_reg_size) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x7FFFFFFF;
        doTestInst(L"LSRV_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000001);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x7FFFFFFF;
        doTestInst(L"LSRV_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"LSRV_32 uses 32bit registers");
    }

    TEST_METHOD(LSRV_32_same_reg) {

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x0;
        doTestInst(L"LSRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x21);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x10;
        doTestInst(L"LSRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x20000000;
        doTestInst(L"LSRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x80000000);
        inst.Rn = 2;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x20000000;
        doTestInst(L"LSRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x0;
        doTestInst(L"LSRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x21);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x10;
        doTestInst(L"LSRV_32 can use the same register");
    }
};

TEST_CLASS(asrv32), BaseTestClass<cpu::instructionID::ASRV_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(ASRV_32_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x80000000;
        doTestInst(L"ASRV_32 does nothing when shift is 0");

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC0000000;
        doTestInst(L"ASRV_32 can shift");

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x1f);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"ASRV_32 can shift");

        SET_REG(GPRegs[1], 0x40000000);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x20000000;
        doTestInst(L"ASRV_32 can shift");
    }

    TEST_METHOD(ASRV_32_mod) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x21);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC0000000;
        doTestInst(L"ASRV_32 shift is mod 32");

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x3456378ABC356721);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC0000000;
        doTestInst(L"ASRV_32 shift is mod 32");
    }

    TEST_METHOD(ASRV_32_reg_size) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"ASRV_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x100000001);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"ASRV_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"ASRV_32 uses 32bit registers");
    }

    TEST_METHOD(ASRV_32_same_reg) {

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x0;
        doTestInst(L"ASRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x21);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x10;
        doTestInst(L"ASRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 1;
        expected.GPRegs[1] = 0xE0000000;
        doTestInst(L"ASRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x80000000);
        inst.Rn = 2;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0xE0000000;
        doTestInst(L"ASRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x0;
        doTestInst(L"ASRV_32 can use the same register");

        SET_REG(GPRegs[1], 0x21);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x10;
        doTestInst(L"ASRV_32 can use the same register");
    }
};

TEST_CLASS(rorv32), BaseTestClass<cpu::instructionID::RORV_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(RORV_32_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x80000001);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x80000001;
        doTestInst(L"RORV_32 does nothing when shift is 0");

        SET_REG(GPRegs[1], 0x80000001);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC0000000;
        doTestInst(L"RORV_32 can rotate");

        SET_REG(GPRegs[1], 0x80000001);
        SET_REG(GPRegs[2], 0x1f);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x3;
        doTestInst(L"RORV_32 can rotate");
    }

    TEST_METHOD(RORV_32_mod) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x80000001);
        SET_REG(GPRegs[2], 0x21);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC0000000;
        doTestInst(L"RORV_32 shift is mod 32");

        SET_REG(GPRegs[1], 0x80000001);
        SET_REG(GPRegs[2], 0x3456378ABC356721);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC0000000;
        doTestInst(L"RORV_32 shift is mod 32");
    }

    TEST_METHOD(RORV_32_reg_size) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x100000001);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x80000000;
        doTestInst(L"RORV_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0x100000001);
        SET_REG(GPRegs[2], 0x100000001);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x80000000;
        doTestInst(L"RORV_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFF;
        doTestInst(L"RORV_32 uses 32bit registers");
    }

    TEST_METHOD(RORV_32_same_reg) {

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x80000000;
        doTestInst(L"RORV_32 can use the same register");

        SET_REG(GPRegs[1], 0x21);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x80000010;
        doTestInst(L"RORV_32 can use the same register");

        SET_REG(GPRegs[1], 0x80000001);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x60000000;
        doTestInst(L"RORV_32 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x80000001);
        inst.Rn = 2;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x60000000;
        doTestInst(L"RORV_32 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x80000000;
        doTestInst(L"RORV_32 can use the same register");

        SET_REG(GPRegs[1], 0x21);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x80000010;
        doTestInst(L"RORV_32 can use the same register");
    }
};

TEST_CLASS(lslv64), BaseTestClass<cpu::instructionID::LSLV_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(LSLV_64_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x1;
        doTestInst(L"LSLV_64 does nothing when shift is 0");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x2;
        doTestInst(L"LSLV_64 can shift");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x3f);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x8000000000000000;
        doTestInst(L"LSLV_64 can shift");
    }

    TEST_METHOD(LSLV_64_mod) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x41);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x2;
        doTestInst(L"LSLV_64 shift is mod 64");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x3456378ABC356741);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x2;
        doTestInst(L"LSLV_64 shift is mod 64");
    }

    TEST_METHOD(LSLV_64_same_reg) {

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x8;
        doTestInst(L"LSLV_64 can use the same register");

        SET_REG(GPRegs[1], 0x41);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x82;
        doTestInst(L"LSLV_64 can use the same register");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x4;
        doTestInst(L"LSLV_64 can use the same register");

        SET_REG(GPRegs[1], 0x1);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 2;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x4;
        doTestInst(L"LSLV_64 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x8;
        doTestInst(L"LSLV_64 can use the same register");

        SET_REG(GPRegs[1], 0x41);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x82;
        doTestInst(L"LSLV_64 can use the same register");
    }
};

TEST_CLASS(lsrv64), BaseTestClass<cpu::instructionID::LSRV_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(LSRV_64_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x8000000000000000;
        doTestInst(L"LSRV_64 does nothing when shift is 0");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x4000000000000000;
        doTestInst(L"LSRV_64 can shift");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x3f);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x1;
        doTestInst(L"LSRV_64 can shift");
    }

    TEST_METHOD(LSRV_64_mod) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x41);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x4000000000000000;
        doTestInst(L"LSRV_64 shift is mod 64");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x3456378ABC356741);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0x4000000000000000;
        doTestInst(L"LSRV_64 shift is mod 64");
    }

    TEST_METHOD(LSRV_64_same_reg) {

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x0;
        doTestInst(L"LSRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x41);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x20;
        doTestInst(L"LSRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x2000000000000000;
        doTestInst(L"LSRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x8000000000000000);
        inst.Rn = 2;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x2000000000000000;
        doTestInst(L"LSRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x0;
        doTestInst(L"LSRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x41);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x20;
        doTestInst(L"LSRV_64 can use the same register");
    }
};

TEST_CLASS(asrv64), BaseTestClass<cpu::instructionID::ASRV_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(ASRV_64_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x8000000000000000;
        doTestInst(L"ASRV_64 does nothing when shift is 0");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC000000000000000;
        doTestInst(L"ASRV_64 can shift");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x3f);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0xFFFFFFFFFFFFFFFF;
        doTestInst(L"ASRV_64 can shift");

        SET_REG(GPRegs[1], 0x4000000000000000);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x2000000000000000;
        doTestInst(L"ASRV_64 can shift");
    }

    TEST_METHOD(ASRV_64_mod) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x41);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC000000000000000;
        doTestInst(L"ASRV_64 shift is mod 64");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x3456378ABC356741);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC000000000000000;
        doTestInst(L"ASRV_64 shift is mod 64");
    }

    TEST_METHOD(ASRV_64_same_reg) {

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x0;
        doTestInst(L"ASRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x41);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x20;
        doTestInst(L"ASRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 1;
        expected.GPRegs[1] = 0xE000000000000000;
        doTestInst(L"ASRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x8000000000000000);
        inst.Rn = 2;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0xE000000000000000;
        doTestInst(L"ASRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x0;
        doTestInst(L"ASRV_64 can use the same register");

        SET_REG(GPRegs[1], 0x41);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x20;
        doTestInst(L"ASRV_64 can use the same register");
    }
};

TEST_CLASS(rorv64), BaseTestClass<cpu::instructionID::RORV_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(RORV_64_basic) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x8000000000000001);
        SET_REG(GPRegs[2], 0);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x8000000000000001;
        doTestInst(L"RORV_64 does nothing when shift is 0");

        SET_REG(GPRegs[1], 0x8000000000000001);
        SET_REG(GPRegs[2], 0x1);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC000000000000000;
        doTestInst(L"RORV_64 can rotate");

        SET_REG(GPRegs[1], 0x8000000000000001);
        SET_REG(GPRegs[2], 0x3f);
        SET_REG(GPRegs[3], 0x2);
        expected.GPRegs[3] = 0x3;
        doTestInst(L"RORV_64 can rotate");
    }

    TEST_METHOD(RORV_64_mod) {
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 3;

        SET_REG(GPRegs[1], 0x8000000000000001);
        SET_REG(GPRegs[2], 0x41);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC000000000000000;
        doTestInst(L"RORV_64 shift is mod 64");

        SET_REG(GPRegs[1], 0x8000000000000001);
        SET_REG(GPRegs[2], 0x3456378ABC356741);
        SET_REG(GPRegs[3], 0x1);
        expected.GPRegs[3] = 0xC000000000000000;
        doTestInst(L"RORV_64 shift is mod 64");
    }

    TEST_METHOD(RORV_64_same_reg) {

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x8000000000000000;
        doTestInst(L"RORV_64 can use the same register");

        SET_REG(GPRegs[1], 0x41);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0x8000000000000020;
        doTestInst(L"RORV_64 can use the same register");

        SET_REG(GPRegs[1], 0x8000000000000001);
        SET_REG(GPRegs[2], 0x2);
        inst.Rn = 1;
        inst.Rm = 2;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x6000000000000000;
        doTestInst(L"RORV_64 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        SET_REG(GPRegs[2], 0x8000000000000001);
        inst.Rn = 2;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x6000000000000000;
        doTestInst(L"RORV_64 can use the same register");

        SET_REG(GPRegs[1], 0x2);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x8000000000000000;
        doTestInst(L"RORV_64 can use the same register");

        SET_REG(GPRegs[1], 0x41);
        inst.Rn = 1;
        inst.Rm = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x8000000000000020;
        doTestInst(L"RORV_64 can use the same register");
    }
};

// CRC* are not implemented

TEST_CLASS(clz32), BaseTestClass<cpu::instructionID::CLZ_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(CLZ_32_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLZ_32 counts leading 0s");

        SET_REG(GPRegs[1], 0x7FFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLZ_32 counts leading 0s");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 32;
        doTestInst(L"CLZ_32 counts leading 0s");
    }

    TEST_METHOD(CLZ_32_reg_size) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLZ_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLZ_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0x7FFFFFFF7FFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLZ_32 uses 32bit registers");
    }

    TEST_METHOD(CLZ_32_same_reg) {
        SET_REG(GPRegs[1], 0);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 32;
        doTestInst(L"CLZ_32 can use the same register");
    }

    TEST_METHOD(CLZ_32_all) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 32; i++) {
            SET_REG(GPRegs[1], setBitTable[i]);
            SET_REG(GPRegs[2], 99);
            expected.GPRegs[2] = 31 - i;
            doTestInst(L"CLZ_32 counts leading 0s in setBitTable");
        }

        for (auto i = 0; i < 33; i++) {
            SET_REG(GPRegs[1], leadingClearBitTable[i]);
            SET_REG(GPRegs[2], 99);
            expected.GPRegs[2] = 32 - i;
            doTestInst(L"CLZ_32 counts leading 0s in leadingClearBitTable");
        }
    }
};

TEST_CLASS(clz64), BaseTestClass<cpu::instructionID::CLZ_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(CLZ_64_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLZ_64 counts leading 0s");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLZ_64 counts leading 0s");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 64;
        doTestInst(L"CLZ_64 counts leading 0s");
    }

    TEST_METHOD(CLZ_64_reg_size) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLZ_64 uses 64bit registers");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLZ_64 uses 64bit registers");

        SET_REG(GPRegs[1], 0x7FFFFFFF7FFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLZ_64 uses 64bit registers");
    }

    TEST_METHOD(CLZ_64_same_reg) {
        SET_REG(GPRegs[1], 0);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 64;
        doTestInst(L"CLZ_64 can use the same register");
    }

    TEST_METHOD(CLZ_64_all) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 64; i++) {
            SET_REG(GPRegs[1], setBitTable[i]);
            SET_REG(GPRegs[2], 99);
            expected.GPRegs[2] = 63 - i;
            doTestInst(L"CLZ_64 counts leading 0s in setBitTable");
        }

        for (auto i = 0; i < 65; i++) {
            SET_REG(GPRegs[1], leadingClearBitTable[i]);
            SET_REG(GPRegs[2], 99);
            expected.GPRegs[2] = 64 - i;
            doTestInst(L"CLZ_64 counts leading 0s in leadingClearBitTable");
        }
    }
};

TEST_CLASS(cls32), BaseTestClass<cpu::instructionID::CLS_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(CLS_32_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLS_32 counts leading 1s");

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLS_32 counts leading 1s");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 32;
        doTestInst(L"CLS_32 counts leading 1s");
    }

    TEST_METHOD(CLS_32_reg_size) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLS_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLS_32 uses 32bit registers");

        SET_REG(GPRegs[1], 0x8000000080000000);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLS_32 uses 32bit registers");
    }

    TEST_METHOD(CLS_32_same_reg) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 32;
        doTestInst(L"CLS_32 can use the same register");
    }

    TEST_METHOD(CLS_32_all) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 32; i++) {
            SET_REG(GPRegs[1], clearBitTable[i]);
            SET_REG(GPRegs[2], 99);
            expected.GPRegs[2] = 31 - i;
            doTestInst(L"CLS_32 counts leading 1s in clearBitTable");
        }

        for (auto i = 0; i < 33; i++) {
            SET_REG(GPRegs[1], leadingSetBitTable[i]);
            SET_REG(GPRegs[2], 99);
            expected.GPRegs[2] = 32 - i;
            doTestInst(L"CLS_32 counts leading 1s in leadingSetBitTable");
        }
    }
};

TEST_CLASS(cls64), BaseTestClass<cpu::instructionID::CLS_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(CLS_64_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLS_64 counts leading 1s");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLS_64 counts leading 1s");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 64;
        doTestInst(L"CLS_64 counts leading 1s");
    }

    TEST_METHOD(CLS_64_reg_size) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 0;
        doTestInst(L"CLS_64 uses 64bit registers");

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLS_64 uses 64bit registers");

        SET_REG(GPRegs[1], 0x8000000080000000);
        SET_REG(GPRegs[2], 99);
        expected.GPRegs[2] = 1;
        doTestInst(L"CLS_64 uses 64bit registers");
    }

    TEST_METHOD(CLS_64_same_reg) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 64;
        doTestInst(L"CLS_64 can use the same register");
    }

    TEST_METHOD(CLS_64_all) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 64; i++) {
            SET_REG(GPRegs[1], clearBitTable[i]);
            SET_REG(GPRegs[2], 99);
            expected.GPRegs[2] = 63 - i;
            doTestInst(L"CLS_64 counts leading 1s in clearBitTable");
        }

        for (auto i = 0; i < 65; i++) {
            SET_REG(GPRegs[1], leadingSetBitTable[i]);
            SET_REG(GPRegs[2], 99);
            expected.GPRegs[2] = 64 - i;
            doTestInst(L"CLS_64 counts leading 1s in leadingSetBitTable");
        }
    }
};

TEST_CLASS(rbit32), BaseTestClass<cpu::instructionID::RBIT_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(RBIT_32_all_bits) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 32; i++) {
            SET_REG(GPRegs[1], 1llu << i);
            SET_REG(GPRegs[2], 0);
            expected.GPRegs[2] = bitTable_RBIT_32[i];
            doTestInst(L"RBIT_32 reverses bits");
        }
    }

    TEST_METHOD(RBIT_32_reg_size) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x1);
        inst.Rn = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0xFFFFFFFF;
        doTestInst(L"RBIT_32 uses 32bit registers");
    }

    TEST_METHOD(RBIT_32_same_reg) {
        SET_REG(GPRegs[1], 0x1);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x80000000;
        doTestInst(L"RBIT_32 can use the same register");
    }
};

TEST_CLASS(rev32), BaseTestClass<cpu::instructionID::REV_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(REV_32_all_bits) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 32; i++) {
            SET_REG(GPRegs[1], 1llu << i);
            SET_REG(GPRegs[2], 0);
            expected.GPRegs[2] = bitTable_REV_32[i];
            doTestInst(L"REV_32 reverses bits");
        }
    }

    TEST_METHOD(REV_32_reg_size) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x1);
        inst.Rn = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0xFFFFFFFF;
        doTestInst(L"REV_32 uses 32bit registers");
    }

    TEST_METHOD(REV_32_same_reg) {
        SET_REG(GPRegs[1], 0x1);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x01000000;
        doTestInst(L"REV_32 can use the same register");
    }
};

TEST_CLASS(rev1632), BaseTestClass<cpu::instructionID::REV16_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(REV16_32_all_bits) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 32; i++) {
            SET_REG(GPRegs[1], 1llu << i);
            SET_REG(GPRegs[2], 0);
            expected.GPRegs[2] = bitTable_REV16_32[i];
            doTestInst(L"REV16_32 reverses bits");
        }
    }

    TEST_METHOD(REV16_32_reg_size) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 0x1);
        inst.Rn = 1;
        inst.Rd = 2;
        expected.GPRegs[2] = 0xFFFFFFFF;
        doTestInst(L"REV16_32 uses 32bit registers");
    }

    TEST_METHOD(REV16_32_same_reg) {
        SET_REG(GPRegs[1], 0x1);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x00010000;
        doTestInst(L"REV16_32 can use the same register");
    }
};

TEST_CLASS(rbit64), BaseTestClass<cpu::instructionID::RBIT_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(RBIT_64_all_bits) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 64; i++) {
            SET_REG(GPRegs[1], 1llu << i);
            SET_REG(GPRegs[2], 0);
            expected.GPRegs[2] = bitTable_RBIT_64[i];
            doTestInst(L"RBIT_64 reverses bits");
        }
    }

    TEST_METHOD(RBIT_64_same_reg) {
        SET_REG(GPRegs[1], 0x1);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x8000000000000000;
        doTestInst(L"RBIT_64 can use the same register");
    }
};

TEST_CLASS(rev64), BaseTestClass<cpu::instructionID::REV_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(REV_64_all_bits) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 64; i++) {
            SET_REG(GPRegs[1], 1llu << i);
            SET_REG(GPRegs[2], 0);
            expected.GPRegs[2] = bitTable_REV_64[i];
            doTestInst(L"REV_64 reverses bits");
        }
    }

    TEST_METHOD(REV_64_same_reg) {
        SET_REG(GPRegs[1], 0x1);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x0100000000000000;
        doTestInst(L"REV_64 can use the same register");
    }
};

TEST_CLASS(rev1664), BaseTestClass<cpu::instructionID::REV16_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(REV16_64_all_bits) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 64; i++) {
            SET_REG(GPRegs[1], 1llu << i);
            SET_REG(GPRegs[2], 0);
            expected.GPRegs[2] = bitTable_REV16_64[i];
            doTestInst(L"REV16_64 reverses bits");
        }
    }

    TEST_METHOD(REV16_64_same_reg) {
        SET_REG(GPRegs[1], 0x1);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x0001000000000000;
        doTestInst(L"REV16_64 can use the same register");
    }
};

TEST_CLASS(rev3264), BaseTestClass<cpu::instructionID::REV32_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(REV16_64_all_bits) {
        inst.Rn = 1;
        inst.Rd = 2;

        for (auto i = 0; i < 64; i++) {
            SET_REG(GPRegs[1], 1llu << i);
            SET_REG(GPRegs[2], 0);
            expected.GPRegs[2] = bitTable_REV32_64[i];
            doTestInst(L"REV16_64 reverses bits");
        }
    }

    TEST_METHOD(REV16_64_same_reg) {
        SET_REG(GPRegs[1], 0x1);
        inst.Rn = 1;
        inst.Rd = 1;
        expected.GPRegs[1] = 0x0000000100000000;
        doTestInst(L"REV16_64 can use the same register");
    }
};

} // namespace InterpreterUnitTest