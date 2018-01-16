#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterpreterUnitTest {

TEST_CLASS(adr), BaseTestClass<cpu::instructionID::ADR> {
    INST_CLASS_METHODS;

    TEST_METHOD(ADR_write) {
        inst.Rd = 1;

        SET_REG(PC, 0);
        inst.immlo = 0x1;
        inst.immhi = 0;
        expected.GPRegs[1] = 1;
        doTestInst(L"ADR writes to register");

        SET_REG(PC, 0);
        inst.immlo = 0x3;
        inst.immhi = 0x3FFFF;
        expected.GPRegs[1] = 0xFFFFF;
        doTestInst(L"ADR writes to register");

        SET_REG(PC, 0);
        inst.immlo = 0;
        inst.immhi = 0x40000;
        expected.GPRegs[1] = 0xFFFFFFFFFFF00000;
        doTestInst(L"ADR uses a signed offset");

        SET_REG(PC, 0);
        inst.immlo = 0x3;
        inst.immhi = 0x40000;
        expected.GPRegs[1] = 0xFFFFFFFFFFF00003;
        doTestInst(L"ADR uses a signed offset");

        SET_REG(PC, 0x12345678);
        inst.immlo = 0x1;
        inst.immhi = 0;
        expected.GPRegs[1] = 0x12345679;
        doTestInst(L"ADR uses an offset from the PC");

        SET_REG(PC, 0x12345678);
        inst.immlo = 0x3;
        inst.immhi = 0x3FFFF;
        expected.GPRegs[1] = 0x12445677;
        doTestInst(L"ADR use an offset from the PC");

        SET_REG(PC, 0x12345678);
        inst.immlo = 0;
        inst.immhi = 0x40000;
        expected.GPRegs[1] = 0x0000000012245678;
        doTestInst(L"ADR uses a signed offset");

        SET_REG(PC, 0x12345678);
        inst.immlo = 0x3;
        inst.immhi = 0x40000;
        expected.GPRegs[1] = 0x000000001224567B;
        doTestInst(L"ADR uses a signed offset");
    }

    TEST_METHOD(ADR_sp) {
        inst.Rd = 31;
        inst.immlo = 0x1;
        inst.immhi = 0;
        doTestInst(L"ADR does not write to SP");
    }
};

TEST_CLASS(adrp), BaseTestClass<cpu::instructionID::ADRP> {
    INST_CLASS_METHODS;

    TEST_METHOD(ADRP_write) {
        inst.Rd = 1;

        SET_REG(PC, 0);
        inst.immlo = 0x1;
        inst.immhi = 0;
        expected.GPRegs[1] = 0x1000;
        doTestInst(L"ADRP writes to register");

        SET_REG(PC, 0);
        inst.immlo = 0x3;
        inst.immhi = 0x3FFFF;
        expected.GPRegs[1] = 0xFFFFF000;
        doTestInst(L"ADRP writes to register");

        SET_REG(PC, 0);
        inst.immlo = 0;
        inst.immhi = 0x40000;
        expected.GPRegs[1] = 0xFFFFFFFF00000000;
        doTestInst(L"ADRP uses a signed offset");

        SET_REG(PC, 0);
        inst.immlo = 0x3;
        inst.immhi = 0x40000;
        expected.GPRegs[1] = 0xFFFFFFFF00003000;
        doTestInst(L"ADRP uses a signed offset");

        SET_REG(PC, 0x12345678);
        inst.immlo = 0;
        inst.immhi = 0;
        expected.GPRegs[1] = 0x12345000;
        doTestInst(L"ADRP returns a page-aligned address");

        SET_REG(PC, 0x12345678);
        inst.immlo = 0x1;
        inst.immhi = 0;
        expected.GPRegs[1] = 0x12346000;
        doTestInst(L"ADRP uses an offset from the PC");

        SET_REG(PC, 0x12345678);
        inst.immlo = 0x3;
        inst.immhi = 0x3FFFF;
        expected.GPRegs[1] = 0x112344000;
        doTestInst(L"ADRP use an offset from the PC");

        SET_REG(PC, 0x12345678);
        inst.immlo = 0;
        inst.immhi = 0x40000;
        expected.GPRegs[1] = 0xFFFFFFFF12345000;
        doTestInst(L"ADRP uses a signed offset");

        SET_REG(PC, 0x12345678);
        inst.immlo = 0x3;
        inst.immhi = 0x40000;
        expected.GPRegs[1] = 0xFFFFFFFF12348000;
        doTestInst(L"ADRP uses a signed offset");
    }

    TEST_METHOD(ADRP_sp) {
        inst.Rd = 31;
        inst.immlo = 0x1;
        inst.immhi = 0;
        doTestInst(L"ADRP does not write to SP");
    }
};

TEST_CLASS(add32imm), BaseTestClass<cpu::instructionID::ADD_32_imm> {
    INST_CLASS_METHODS;

    TEST_METHOD(ADD_32_imm_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        expected.GPRegs[2] = 5;
        doTestInst(L"ADD_32_imm adds");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADD_32_imm adds");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 1;
        doTestInst(L"ADD_32_imm adds");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x1004;
        doTestInst(L"ADD_32_imm adds");
    }

    TEST_METHOD(ADD_32_imm_shift) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFF;
        doTestInst(L"ADD_32_imm has a shift");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x1004;
        doTestInst(L"ADD_32_imm has a shift");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF000;
        doTestInst(L"ADD_32_imm has a shift");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF005;
        doTestInst(L"ADD_32_imm has a shift");
    }

    TEST_METHOD(ADD_32_imm_sp) {
        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 5;
        doTestInst(L"ADD_32_imm uses SP");

        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADD_32_imm uses SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 0;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 5;
        doTestInst(L"ADD_32_imm writes to SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 1;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 6;
        doTestInst(L"ADD_32_imm writes to SP");
    }

    TEST_METHOD(ADD_32_imm_reg_size) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFF00000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 1;
        doTestInst(L"ADD_32_imm uses 32 bit regs");

        SET_REG(GPRegs[1], 0xFFFFFFFF00000005);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADD_32_imm uses 32 bit regs");

        SET_REG(GPRegs[2], 7);
        SET_REG(SP, 0xFFFFFFF0FFFFFFF0);
        inst._10_21 = 5;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 0xFFFFFFF5;
        doTestInst(L"ADD_32_imm uses 32 bits of SP as well");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFF0);
        SET_REG(SP, 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 5;
        inst.Rn = 1;
        inst.Rd = 31;
        expected.SP = 0xFFFFFFF5;
        doTestInst(L"ADD_32_imm uses 32 bits of SP as well");
    }

    TEST_METHOD(ADD_32_imm_overflow) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        doTestInst(L"ADD_32_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFF001);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        doTestInst(L"ADD_32_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF;
        doTestInst(L"ADD_32_imm overflows");

        SET_REG(GPRegs[1], 0xFF001FFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF;
        doTestInst(L"ADD_32_imm overflows");
    }

    TEST_METHOD(ADD_32_imm_same) {
        inst.Rn = 1;
        inst.Rd = 1;

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"ADD_32_imm can use the same register");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 0;
        expected.GPRegs[1] = 1;
        doTestInst(L"ADD_32_imm can use the same register");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 5;
        expected.GPRegs[1] = 6;
        doTestInst(L"ADD_32_imm can use the same register");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[1] = 0xFFFFFFFF;
        doTestInst(L"ADD_32_imm still clears upper bits when using same register and adding 0");
    }
};

TEST_CLASS(add64imm), BaseTestClass<cpu::instructionID::ADD_64_imm> {
    INST_CLASS_METHODS;

    TEST_METHOD(ADD_64_imm_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        expected.GPRegs[2] = 5;
        doTestInst(L"ADD_64_imm adds");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADD_64_imm adds");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 1;
        doTestInst(L"ADD_64_imm adds");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x1004;
        doTestInst(L"ADD_64_imm adds");
    }

    TEST_METHOD(ADD_64_imm_shift) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFF;
        doTestInst(L"ADD_64_imm has a shift");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x1004;
        doTestInst(L"ADD_64_imm has a shift");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF000;
        doTestInst(L"ADD_64_imm has a shift");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF005;
        doTestInst(L"ADD_64_imm has a shift");
    }

    TEST_METHOD(ADD_64_imm_sp) {
        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 5;
        doTestInst(L"ADD_64_imm uses SP");

        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADD_64_imm uses SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 0;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 5;
        doTestInst(L"ADD_64_imm writes to SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 1;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 6;
        doTestInst(L"ADD_64_imm writes to SP");
    }

    TEST_METHOD(ADD_64_imm_overflow) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        doTestInst(L"ADD_64_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFF001);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        doTestInst(L"ADD_64_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF;
        doTestInst(L"ADD_64_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFFFFFFF001FFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF;
        doTestInst(L"ADD_64_imm overflows");
    }

    TEST_METHOD(ADD_64_imm_same) {
        inst.Rn = 1;
        inst.Rd = 1;

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"ADD_64_imm can use the same register");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 0;
        expected.GPRegs[1] = 1;
        doTestInst(L"ADD_64_imm can use the same register");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 5;
        expected.GPRegs[1] = 6;
        doTestInst(L"ADD_64_imm can use the same register");
    }
};

TEST_CLASS(sub32imm), BaseTestClass<cpu::instructionID::SUB_32_imm> {
    INST_CLASS_METHODS;

    TEST_METHOD(SUB_32_imm_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        expected.GPRegs[2] = 5;
        doTestInst(L"SUB_32_imm subtracts");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 4;
        doTestInst(L"SUB_32_imm subtracts");

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 1;
        doTestInst(L"SUB_32_imm subtracts");
    }

    TEST_METHOD(SUB_32_imm_shift) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 1;
        doTestInst(L"SUB_32_imm has a shift");

        SET_REG(GPRegs[1], 0x1000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFF001;
        doTestInst(L"SUB_32_imm has a shift");

        SET_REG(GPRegs[1], 0x1000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x1000;
        doTestInst(L"SUB_32_imm has a shift");
    }

    TEST_METHOD(SUB_32_imm_sp) {
        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 5;
        doTestInst(L"SUB_32_imm uses SP");

        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 4;
        doTestInst(L"SUB_32_imm uses SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 0;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 5;
        doTestInst(L"SUB_32_imm writes to SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 1;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 4;
        doTestInst(L"SUB_32_imm writes to SP");
    }

    TEST_METHOD(SUB_32_imm_reg_size) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0xFFFFFFFE;
        doTestInst(L"SUB_32_imm uses 32 bit regs");

        SET_REG(GPRegs[1], 0xFFFFFFFF00000005);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 4;
        doTestInst(L"SUB_32_imm uses 32 bit regs");

        SET_REG(GPRegs[2], 7);
        SET_REG(SP, 0xFFFFFFF0FFFFFFF0);
        inst._10_21 = 5;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 0xFFFFFFEB;
        doTestInst(L"SUB_32_imm uses 32 bits of SP as well");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFF0);
        SET_REG(SP, 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 5;
        inst.Rn = 1;
        inst.Rd = 31;
        expected.SP = 0xFFFFFFEB;
        doTestInst(L"SUB_32_imm uses 32 bits of SP as well");
    }

    TEST_METHOD(SUB_32_imm_underflow) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x7FFFFFFF;
        doTestInst(L"SUB_32_imm underflows");

        SET_REG(GPRegs[1], 0x80000FFE);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x7FFFFFFF;
        doTestInst(L"SUB_32_imm underflows");

        SET_REG(GPRegs[1], 0x80000FFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFF;
        doTestInst(L"SUB_32_imm underflows");

        SET_REG(GPRegs[1], 0x80FFEFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFF;
        doTestInst(L"SUB_32_imm underflows");
    }

    TEST_METHOD(SUB_32_imm_same) {
        inst.Rn = 1;
        inst.Rd = 1;

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 0;
        expected.GPRegs[1] = 1;
        doTestInst(L"SUB_32_imm can use the same register");

        SET_REG(GPRegs[1], 2);
        inst._10_21 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"SUB_32_imm can use the same register");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[1] = 0xFFFFFFFF;
        doTestInst(L"SUB_32_imm still clears upper bits when using same register and subtracting 0");
    }
};

TEST_CLASS(sub64imm), BaseTestClass<cpu::instructionID::SUB_64_imm> {
    INST_CLASS_METHODS;

    TEST_METHOD(SUB_64_imm_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        expected.GPRegs[2] = 5;
        doTestInst(L"SUB_64_imm subtracts");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 4;
        doTestInst(L"SUB_64_imm subtracts");

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 1;
        doTestInst(L"SUB_64_imm subtracts");
    }

    TEST_METHOD(SUB_64_imm_shift) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 1;
        doTestInst(L"SUB_64_imm has a shift");

        SET_REG(GPRegs[1], 0x1000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFF001;
        doTestInst(L"SUB_64_imm has a shift");

        SET_REG(GPRegs[1], 0x1000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x1000;
        doTestInst(L"SUB_64_imm has a shift");
    }

    TEST_METHOD(SUB_64_imm_sp) {
        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 5;
        doTestInst(L"SUB_64_imm uses SP");

        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 4;
        doTestInst(L"SUB_64_imm uses SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 0;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 5;
        doTestInst(L"SUB_64_imm writes to SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 1;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 4;
        doTestInst(L"SUB_64_imm writes to SP");
    }

    TEST_METHOD(SUB_64_imm_underflow) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        doTestInst(L"SUB_64_imm underflows");

        SET_REG(GPRegs[1], 0x8000000000000FFE);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        doTestInst(L"SUB_64_imm underflows");

        SET_REG(GPRegs[1], 0x8000000000000FFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        doTestInst(L"SUB_64_imm underflows");

        SET_REG(GPRegs[1], 0x8000000000FFEFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        doTestInst(L"SUB_64_imm underflows");
    }

    TEST_METHOD(SUB_64_imm_same) {
        inst.Rn = 1;
        inst.Rd = 1;

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 0;
        expected.GPRegs[1] = 1;
        doTestInst(L"SUB_64_imm can use the same register");

        SET_REG(GPRegs[1], 2);
        inst._10_21 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"SUB_64_imm can use the same register");
    }
};

TEST_CLASS(adds32imm), BaseTestClass<cpu::instructionID::ADDS_32_imm> {
    INST_CLASS_METHODS;

    TEST_METHOD(ADDS_32_imm_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        expected.GPRegs[2] = 5;
        doTestInst(L"ADDS_32_imm adds");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADDS_32_imm adds");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 1;
        doTestInst(L"ADDS_32_imm adds");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x1004;
        doTestInst(L"ADDS_32_imm adds");
    }

    TEST_METHOD(ADDS_32_imm_shift) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFF;
        doTestInst(L"ADDS_32_imm has a shift");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x1004;
        doTestInst(L"ADDS_32_imm has a shift");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF000;
        doTestInst(L"ADDS_32_imm has a shift");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF005;
        doTestInst(L"ADDS_32_imm has a shift");
    }

    TEST_METHOD(ADDS_32_imm_sp) {
        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 5;
        doTestInst(L"ADDS_32_imm uses SP");

        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADDS_32_imm uses SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 0;
        inst.Rn = 2;
        inst.Rd = 31;
        doTestInst(L"ADDS_32_imm does not write to SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 1;
        inst.Rn = 2;
        inst.Rd = 31;
        doTestInst(L"ADDS_32_imm does not writes to SP");
    }

    TEST_METHOD(ADDS_32_imm_reg_size) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFF00000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 1;
        doTestInst(L"ADDS_32_imm uses 32 bit regs");

        SET_REG(GPRegs[1], 0xFFFFFFFF00000005);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADDS_32_imm uses 32 bit regs");

        SET_REG(GPRegs[2], 7);
        SET_REG(SP, 0xFFFFFFF0FFFFFFF0);
        inst._10_21 = 5;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 0xFFFFFFF5;
        expected.PSTATE.N = true;
        doTestInst(L"ADD_32_imm uses 32 bits of SP as well");
    }

    TEST_METHOD(ADDS_32_imm_overflow) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFF001);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm overflows");

        SET_REG(GPRegs[1], 0xFF001FFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm overflows");
    }

    TEST_METHOD(ADDS_32_imm_same) {
        inst.Rn = 1;
        inst.Rd = 1;

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"ADDS_32_imm can use the same register");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 0;
        expected.GPRegs[1] = 1;
        doTestInst(L"ADDS_32_imm can use the same register");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 5;
        expected.GPRegs[1] = 6;
        doTestInst(L"ADDS_32_imm can use the same register");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[1] = 0xFFFFFFFF;
        expected.PSTATE.N = true;
        doTestInst(L"ADDS_32_imm still clears upper bits when using same register and adding 0");
    }

    TEST_METHOD(ADDS_32_imm_flag_N) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0xFFFFFFFF;
        expected.PSTATE.N = true;
        doTestInst(L"ADDS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        doTestInst(L"ADDS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0x7FFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0x7FFFF001);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0x7FFFF000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0x7F001000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_32_imm sets the N flag");
    }

    TEST_METHOD(ADDS_32_imm_flag_Z) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        doTestInst(L"ADDS_32_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFFFF001);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFFFF000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFF001000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm sets the Z flag");
    }

    TEST_METHOD(ADDS_32_imm_flag_C) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFFFFF001);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFFFFF000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFF001000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_32_imm sets the C flag");
    }

    TEST_METHOD(ADDS_32_imm_flag_V) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x7FFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_32_imm sets the V flag");

        SET_REG(GPRegs[1], 0x7FFFF001);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_32_imm sets the V flag");

        SET_REG(GPRegs[1], 0x7FFFF000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_32_imm sets the V flag");

        SET_REG(GPRegs[1], 0x7F001000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_32_imm sets the V flag");
    }
};

TEST_CLASS(adds64imm), BaseTestClass<cpu::instructionID::ADDS_64_imm> {
    INST_CLASS_METHODS;

    TEST_METHOD(ADDS_64_imm_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        expected.GPRegs[2] = 5;
        doTestInst(L"ADDS_64_imm adds");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADDS_64_imm adds");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 1;
        doTestInst(L"ADDS_64_imm adds");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x1004;
        doTestInst(L"ADDS_64_imm adds");
    }

    TEST_METHOD(ADDS_64_imm_shift) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFF;
        doTestInst(L"ADDS_64_imm has a shift");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x1004;
        doTestInst(L"ADDS_64_imm has a shift");

        SET_REG(GPRegs[1], 0);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF000;
        doTestInst(L"ADDS_64_imm has a shift");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF005;
        doTestInst(L"ADDS_64_imm has a shift");
    }

    TEST_METHOD(ADDS_64_imm_sp) {
        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 5;
        doTestInst(L"ADDS_64_imm uses SP");

        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 6;
        doTestInst(L"ADDS_64_imm uses SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 0;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 7;
        doTestInst(L"ADDS_64_imm does not write to SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 1;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 7;
        doTestInst(L"ADDS_64_imm does not write to SP");
    }

    TEST_METHOD(ADDS_64_imm_overflow) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFF001);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm overflows");

        SET_REG(GPRegs[1], 0xFFFFFFFFFF001FFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFF;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm overflows");
    }

    TEST_METHOD(ADDS_64_imm_same) {
        inst.Rn = 1;
        inst.Rd = 1;

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"ADDS_64_imm can use the same register");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 0;
        expected.GPRegs[1] = 1;
        doTestInst(L"ADDS_64_imm can use the same register");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 5;
        expected.GPRegs[1] = 6;
        doTestInst(L"ADDS_64_imm can use the same register");
    }

    TEST_METHOD(ADDS_64_imm_flag_N) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0xFFFFFFFFFFFFFFFF;
        expected.PSTATE.N = true;
        doTestInst(L"ADDS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        doTestInst(L"ADDS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFF001);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFF000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0x7FFFFFFFFF001000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_64_imm sets the N flag");
    }

    TEST_METHOD(ADDS_64_imm_flag_Z) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        doTestInst(L"ADDS_64_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFF001);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFF000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFFFFFFFFF001000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm sets the Z flag");
    }

    TEST_METHOD(ADDS_64_imm_flag_C) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFF001);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFF000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFFFFFFFFFF001000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"ADDS_64_imm sets the C flag");
    }

    TEST_METHOD(ADDS_64_imm_flag_V) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_64_imm sets the V flag");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFF001);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_64_imm sets the V flag");

        SET_REG(GPRegs[1], 0x7FFFFFFFFFFFF000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_64_imm sets the V flag");

        SET_REG(GPRegs[1], 0x7FFFFFFFFF001000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.V = true;
        doTestInst(L"ADDS_64_imm sets the V flag");
    }
};

TEST_CLASS(subs32imm), BaseTestClass<cpu::instructionID::SUBS_32_imm> {
    INST_CLASS_METHODS;

    TEST_METHOD(SUBS_32_imm_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        expected.GPRegs[2] = 5;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm subtracts");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 4;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm subtracts");

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 1;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm subtracts");
    }

    TEST_METHOD(SUBS_32_imm_shift) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 1;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm has a shift");

        SET_REG(GPRegs[1], 0x1000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFF001;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm has a shift");

        SET_REG(GPRegs[1], 0x1000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x1000;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm has a shift");
    }

    TEST_METHOD(SUBS_32_imm_sp) {
        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 5;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm uses SP");

        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 4;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm uses SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 0;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 7;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm does not write to SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 1;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 7;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm does not write to SP");
    }

    TEST_METHOD(SUBS_32_imm_reg_size) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0xFFFFFFFE;
        expected.PSTATE.C = true;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_32_imm uses 32 bit regs");

        SET_REG(GPRegs[1], 0xFFFFFFFF00000005);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 4;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm uses 32 bit regs");

        SET_REG(GPRegs[2], 7);
        SET_REG(SP, 0xFFFFFFF0FFFFFFF0);
        inst._10_21 = 5;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 0xFFFFFFEB;
        expected.PSTATE.C = true;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_32_imm uses 32 bits of SP as well");
    }

    TEST_METHOD(SUBS_32_imm_underflow) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x80000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x7FFFFFFF;
        expected.PSTATE.C = true;
        expected.PSTATE.V = true;
        doTestInst(L"SUBS_32_imm underflows");

        SET_REG(GPRegs[1], 0x80000FFE);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x7FFFFFFF;
        expected.PSTATE.C = true;
        expected.PSTATE.V = true;
        doTestInst(L"SUBS_32_imm underflows");

        SET_REG(GPRegs[1], 0x80000FFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFF;
        expected.PSTATE.C = true;
        expected.PSTATE.V = true;
        doTestInst(L"SUBS_32_imm underflows");

        SET_REG(GPRegs[1], 0x80FFEFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFF;
        expected.PSTATE.C = true;
        expected.PSTATE.V = true;
        doTestInst(L"SUBS_32_imm underflows");
    }

    TEST_METHOD(SUBS_32_imm_same) {
        inst.Rn = 1;
        inst.Rd = 1;

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 0;
        expected.GPRegs[1] = 1;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm can use the same register");

        SET_REG(GPRegs[1], 2);
        inst._10_21 = 1;
        expected.GPRegs[1] = 1;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm can use the same register");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[1] = 0xFFFFFFFF;
        expected.PSTATE.C = true;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_32_imm still clears upper bits when using same register and subtracting 0");
    }

    TEST_METHOD(SUBS_32_imm_flag_N) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0xFFFFFFFF;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0xFFFFFFFF;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFFFF001;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0xFFFFFFFE;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFFFF000;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_32_imm sets the N flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFF001000;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_32_imm sets the N flag");
    }

    TEST_METHOD(SUBS_32_imm_flag_Z) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the Z flag");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFF);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the Z flag");

        SET_REG(GPRegs[1], 0x1000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFF000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the Z flag");
    }

    TEST_METHOD(SUBS_32_imm_flag_C) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0xFFFFFFFF;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0x80000000;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFFF);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x7FFFFFFF;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x7FFFF001;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFF000;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7F001000;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0x1000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFFF000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the C flag");
    }

    TEST_METHOD(SUBS_32_imm_flag_V) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x7FFFFFFF;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the V flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x7FFFF001;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the V flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFF000;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the V flag");

        SET_REG(GPRegs[1], 0x80000000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7F001000;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_32_imm sets the V flag");
    }
};

TEST_CLASS(subs64imm), BaseTestClass<cpu::instructionID::SUBS_64_imm> {
    INST_CLASS_METHODS;

    TEST_METHOD(SUBS_64_imm_basic) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        expected.GPRegs[2] = 5;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm subtracts");

        SET_REG(GPRegs[1], 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 4;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm subtracts");

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 1;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm subtracts");
    }

    TEST_METHOD(SUBS_64_imm_shift) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x1000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 1;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm has a shift");

        SET_REG(GPRegs[1], 0x1000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFF001;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm has a shift");

        SET_REG(GPRegs[1], 0x1000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x1000;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm has a shift");
    }

    TEST_METHOD(SUBS_64_imm_sp) {
        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 5;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm uses SP");

        SET_REG(SP, 5);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.Rn = 31;
        inst.Rd = 2;
        expected.GPRegs[2] = 4;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm uses SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 0;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 7;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm does not write to SP");

        SET_REG(SP, 7);
        SET_REG(GPRegs[2], 5);
        inst._10_21 = 1;
        inst.Rn = 2;
        inst.Rd = 31;
        expected.SP = 7;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm does not write to SP");
    }

    TEST_METHOD(SUBS_64_imm_underflow) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x8000000000000000);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        expected.PSTATE.C = true;
        expected.PSTATE.V = true;
        doTestInst(L"SUBS_64_imm underflows");

        SET_REG(GPRegs[1], 0x8000000000000FFE);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        expected.PSTATE.C = true;
        expected.PSTATE.V = true;
        doTestInst(L"SUBS_64_imm underflows");

        SET_REG(GPRegs[1], 0x8000000000000FFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        expected.PSTATE.C = true;
        expected.PSTATE.V = true;
        doTestInst(L"SUBS_64_imm underflows");

        SET_REG(GPRegs[1], 0x8000000000FFEFFF);
        SET_REG(GPRegs[2], 7);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        expected.PSTATE.C = true;
        expected.PSTATE.V = true;
        doTestInst(L"SUBS_64_imm underflows");
    }

    TEST_METHOD(SUBS_64_imm_same) {
        inst.Rn = 1;
        inst.Rd = 1;

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 0;
        expected.GPRegs[1] = 1;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm can use the same register");

        SET_REG(GPRegs[1], 2);
        inst._10_21 = 1;
        expected.GPRegs[1] = 1;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm can use the same register");
    }

    TEST_METHOD(SUBS_64_imm_flag_N) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0xFFFFFFFFFFFFFFFF;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0xFFFFFFFFFFFFFFFF;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0xFFFFFFFFFFFFF001;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0xFFFFFFFFFFFFFFFE;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFFFFFFFFFFFF000;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_64_imm sets the N flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0xFFFFFFFFFF001000;
        expected.PSTATE.N = true;
        doTestInst(L"SUBS_64_imm sets the N flag");
    }

    TEST_METHOD(SUBS_64_imm_flag_Z) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the Z flag");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFF);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the Z flag");

        SET_REG(GPRegs[1], 0x1000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the Z flag");

        SET_REG(GPRegs[1], 0xFFF000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the Z flag");
    }

    TEST_METHOD(SUBS_64_imm_flag_C) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0xFFFFFFFFFFFFFFFF;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0x8000000000000000;
        expected.PSTATE.N = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0);
        inst._10_21 = 0;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 1);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFFF);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFF001;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFF000;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFF001000;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0x1000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");

        SET_REG(GPRegs[1], 0xFFF000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0;
        expected.PSTATE.Z = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the C flag");
    }

    TEST_METHOD(SUBS_64_imm_flag_V) {
        inst.Rn = 1;
        inst.Rd = 2;

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFFFFF;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the V flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 0xFFF;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFF001;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the V flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 1;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFFFFF000;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the V flag");

        SET_REG(GPRegs[1], 0x8000000000000000);
        inst._10_21 = 0xFFF;
        inst.shift = 1;
        expected.GPRegs[2] = 0x7FFFFFFFFF001000;
        expected.PSTATE.V = true;
        expected.PSTATE.C = true;
        doTestInst(L"SUBS_64_imm sets the V flag");
    }
};

TEST_CLASS(movz32), BaseTestClass<cpu::instructionID::MOVZ_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(MOVZ_32_write) {
        inst.Rd = 1;

        inst._5_20 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"MOVZ_32 writes to register");

        inst._5_20 = 0;
        expected.GPRegs[1] = 0;
        doTestInst(L"MOVZ_32 writes to register");

        inst._5_20 = 0xFFFF;
        expected.GPRegs[1] = 0xFFFF;
        doTestInst(L"MOVZ_32 writes to register");
    }

    TEST_METHOD(MOVZ_32_reg_size) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;
        expected.GPRegs[1] = 0xFFFF;
        doTestInst(L"MOVZ_32 zeroes upper bits of register");
    }

    TEST_METHOD(MOVZ_32_shift) {
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;
        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFF0000;
        doTestInst(L"MOVZ_32 writes to upper 16 bits with a shift");
    }

    TEST_METHOD(MOVZ_32_clear_with_shift) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;
        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFF0000;
        doTestInst(L"MOVZ_32 clears lower 16 bits when there is a shift");
    }

    TEST_METHOD(MOVZ_32_sp) {
        inst.Rd = 31;
        inst._5_20 = 1;
        doTestInst(L"MOVZ_32 does not write to SP");
    }
};

TEST_CLASS(movz64), BaseTestClass<cpu::instructionID::MOVZ_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(MOVZ_64_write) {
        inst.Rd = 1;

        inst._5_20 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"MOVZ_64 writes to register");

        inst._5_20 = 0;
        expected.GPRegs[1] = 0;
        doTestInst(L"MOVZ_64 writes to register");

        inst._5_20 = 0xFFFF;
        expected.GPRegs[1] = 0xFFFF;
        doTestInst(L"MOVZ_64 writes to register");
    }

    TEST_METHOD(MOVZ_64_shift) {
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;

        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFF0000;
        doTestInst(L"MOVZ_64 writes to upper bits with a shift");

        inst.hw = 2;
        expected.GPRegs[1] = 0xFFFF00000000;
        doTestInst(L"MOVZ_64 writes to upper bits with a shift");

        inst.hw = 3;
        expected.GPRegs[1] = 0xFFFF000000000000;
        doTestInst(L"MOVZ_64 writes to upper bits with a shift");
    }

    TEST_METHOD(MOVZ_64_clear_with_shift) {
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFF0000;
        doTestInst(L"MOVZ_64 clears lower bits when there is a shift");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.hw = 2;
        expected.GPRegs[1] = 0xFFFF00000000;
        doTestInst(L"MOVZ_64 clears lower bits when there is a shift");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.hw = 3;
        expected.GPRegs[1] = 0xFFFF000000000000;
        doTestInst(L"MOVZ_64 clears lower bits when there is a shift");
    }

    TEST_METHOD(MOVZ_64_sp) {
        inst.Rd = 31;
        inst._5_20 = 1;
        doTestInst(L"MOVZ_64 does not write to SP");
    }
};

TEST_CLASS(movk32), BaseTestClass<cpu::instructionID::MOVK_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(MOVK_32_write) {
        inst.Rd = 1;

        inst._5_20 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"MOVK_32 writes to register");

        inst._5_20 = 0;
        expected.GPRegs[1] = 0;
        doTestInst(L"MOVK_32 writes to register");

        inst._5_20 = 0xFFFF;
        expected.GPRegs[1] = 0xFFFF;
        doTestInst(L"MOVK_32 writes to register");
    }

    TEST_METHOD(MOVK_32_reg_size) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;
        expected.GPRegs[1] = 0xFFFFFFFF;
        doTestInst(L"MOVK_32 zeroes upper bits of register");
    }

    TEST_METHOD(MOVK_32_shift) {
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;
        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFF0000;
        doTestInst(L"MOVK_32 writes to upper 16 bits with a shift");
    }

    TEST_METHOD(MOVK_32_keep_with_shift) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;
        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFFFFFF;
        doTestInst(L"MOVK_32 does not affect lower 16 bits with a shift");
    }

    TEST_METHOD(MOVK_32_sp) {
        inst.Rd = 31;
        inst._5_20 = 1;
        doTestInst(L"MOVK_32 does not write to SP");
    }
};

TEST_CLASS(movk64), BaseTestClass<cpu::instructionID::MOVK_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(MOVK_64_write) {
        inst.Rd = 1;

        inst._5_20 = 1;
        expected.GPRegs[1] = 1;
        doTestInst(L"MOVK_64 writes to register");

        inst._5_20 = 0;
        expected.GPRegs[1] = 0;
        doTestInst(L"MOVK_64 writes to register");

        inst._5_20 = 0xFFFF;
        expected.GPRegs[1] = 0xFFFF;
        doTestInst(L"MOVK_64 writes to register");
    }

    TEST_METHOD(MOVK_64_shift) {
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;

        SET_REG(GPRegs[1], 0);
        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFF0000;
        doTestInst(L"MOVK_64 writes to upper bits with a shift");

        SET_REG(GPRegs[1], 0);
        inst.hw = 2;
        expected.GPRegs[1] = 0xFFFF00000000;
        doTestInst(L"MOVK_64 writes to upper bits with a shift");

        SET_REG(GPRegs[1], 0);
        inst.hw = 3;
        expected.GPRegs[1] = 0xFFFF000000000000;
        doTestInst(L"MOVK_64 writes to upper bits with a shift");
    }

    TEST_METHOD(MOVK_64_keep_with_shift) {
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;

        SET_REG(GPRegs[1], 0xFFFF);
        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFFFFFF;
        doTestInst(L"MOVK_64 does not affect lower bits with a shift");

        SET_REG(GPRegs[1], 0xFFFFFFFF);
        inst.hw = 2;
        expected.GPRegs[1] = 0xFFFFFFFFFFFF;
        doTestInst(L"MOVK_64 does not affect lower bits with a shift");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFF);
        inst.hw = 3;
        expected.GPRegs[1] = 0xFFFFFFFFFFFFFFFF;
        doTestInst(L"MOVK_64 does not affect lower bits with a shift");
    }

    TEST_METHOD(MOVK_64_sp) {
        inst.Rd = 31;
        inst._5_20 = 1;
        doTestInst(L"MOVK_64 does not write to SP");
    }
};

TEST_CLASS(movn32), BaseTestClass<cpu::instructionID::MOVN_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(MOVN_32_write) {
        inst.Rd = 1;

        inst._5_20 = 1;
        expected.GPRegs[1] = 0xFFFFFFFE;
        doTestInst(L"MOVN_32 writes to register");

        inst._5_20 = 0;
        expected.GPRegs[1] = 0xFFFFFFFF;
        doTestInst(L"MOVN_32 writes to register");

        inst._5_20 = 0xFFFF;
        expected.GPRegs[1] = 0xFFFF0000;
        doTestInst(L"MOVN_32 writes to register");
    }

    TEST_METHOD(MOVN_32_reg_size) {
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst.Rd = 1;
        inst._5_20 = 0;
        expected.GPRegs[1] = 0xFFFFFFFF;
        doTestInst(L"MOVN_32 zeroes upper bits of register");
    }

    TEST_METHOD(MOVN_32_shift) {
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;
        inst.hw = 1;
        expected.GPRegs[1] = 0x0000FFFF;
        doTestInst(L"MOVN_32 writes to upper 16 bits with a shift");
    }

    TEST_METHOD(MOVN_32_clear_with_shift) {
        SET_REG(GPRegs[1], 0x12345678);
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;
        inst.hw = 1;
        expected.GPRegs[1] = 0x0000FFFF;
        doTestInst(L"MOVN_32 overrides lower 16 bits when there is a shift");
    }

    TEST_METHOD(MOVN_32_sp) {
        inst.Rd = 31;
        inst._5_20 = 1;
        doTestInst(L"MOVN_32 does not write to SP");
    }
};

TEST_CLASS(movn64), BaseTestClass<cpu::instructionID::MOVN_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(MOVN_64_write) {
        inst.Rd = 1;

        inst._5_20 = 1;
        expected.GPRegs[1] = 0xFFFFFFFFFFFFFFFE;
        doTestInst(L"MOVN_64 writes to register");

        inst._5_20 = 0;
        expected.GPRegs[1] = 0xFFFFFFFFFFFFFFFF;
        doTestInst(L"MOVN_64 writes to register");

        inst._5_20 = 0xFFFF;
        expected.GPRegs[1] = 0xFFFFFFFFFFFF0000;
        doTestInst(L"MOVN_64 writes to register");
    }

    TEST_METHOD(MOVN_64_shift) {
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;

        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFFFFFF0000FFFF;
        doTestInst(L"MOVN_64 writes to upper bits with a shift");

        inst.hw = 2;
        expected.GPRegs[1] = 0xFFFF0000FFFFFFFF;
        doTestInst(L"MOVN_64 writes to upper bits with a shift");

        inst.hw = 3;
        expected.GPRegs[1] = 0x0000FFFFFFFFFFFF;
        doTestInst(L"MOVN_64 writes to upper bits with a shift");
    }

    TEST_METHOD(MOVN_64_clear_with_shift) {
        inst.Rd = 1;
        inst._5_20 = 0xFFFF;

        SET_REG(GPRegs[1], 0x123456789ABCDEF);
        inst.hw = 1;
        expected.GPRegs[1] = 0xFFFFFFFF0000FFFF;
        doTestInst(L"MOVN_64 overrides lower bits when there is a shift");

        SET_REG(GPRegs[1], 0x123456789ABCDEF);
        inst.hw = 2;
        expected.GPRegs[1] = 0xFFFF0000FFFFFFFF;
        doTestInst(L"MOVN_64 overrides lower bits when there is a shift");

        SET_REG(GPRegs[1], 0x123456789ABCDEF);
        inst.hw = 3;
        expected.GPRegs[1] = 0x0000FFFFFFFFFFFF;
        doTestInst(L"MOVN_64 overrides lower bits when there is a shift");
    }

    TEST_METHOD(MOVN_64_sp) {
        inst.Rd = 31;
        inst._5_20 = 1;
        doTestInst(L"MOVN_64 does not write to SP");
    }
};

} // namespace InterpreterUnitTest