#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterpreterUnitTest {

TEST_CLASS(bcond), BaseTestClass<cpu::instructionID::B_cond> {
    INST_CLASS_METHODS;

    TEST_METHOD(B_cond_branch) {
        inst.cond = static_cast<uint32_t>(Condition::AL);

        inst._5_23 = 0;
        expected.nextPC = PC_DEFAULT;
        doTestInst(L"B.cond jumps to itself when imm == 0");

        SET_REG(PC, 0x1000);
        inst._5_23 = 2;
        expected.nextPC = 0x1008;
        doTestInst(L"B.cond is in increments of 4");

        SET_REG(PC, 0x1000);
        inst._5_23 = 0x3FFFF;
        expected.nextPC = 0x100FFC;
        doTestInst(L"B.cond is in increments of 4");

        SET_REG(PC, 0x1000);
        inst._5_23 = 0x40000;
        expected.nextPC = 0xFFFFFFFFFFF01000;
        doTestInst(L"B.cond has a signed offset");

        SET_REG(PC, 0x1000);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFC;
        doTestInst(L"B.cond has a signed offset");

        SET_REG(PSTATE.C, false);
        inst.cond = static_cast<uint32_t>(Condition::CS);
        inst._5_23 = 0x1;
        doTestInst(L"B.cond does nothing when the condition is not met");
    }

    TEST_METHOD(B_cond_overflow) {
        inst.cond = static_cast<uint32_t>(Condition::AL);

        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        inst._5_23 = 0x1;
        expected.nextPC = 0;
        doTestInst(L"B.cond wraps around on overflow");
    }

    TEST_METHOD(B_cond_underflow) {
        inst.cond = static_cast<uint32_t>(Condition::AL);

        SET_REG(PC, 0);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFFFFFFFFFFFFFFC;
        doTestInst(L"B.cond wraps around on underflow");
    }

    TEST_METHOD(B_cond_all) {
        inst._5_23 = 0x2;

        auto setupWhenCondMet = [this]() {
            SET_REG(PC, 0x1000);
            SET_REG(nextPC, 0x1004);
            expected.nextPC = 0x1008;
        };
        auto setupWhenCondNotMet = [this]() {
            SET_REG(PC, 0x1000);
            SET_REG(nextPC, 0x1004);
        };
        testAllConditions(setupWhenCondMet, setupWhenCondNotMet);
    }
};

TEST_CLASS(br), BaseTestClass<cpu::instructionID::BR> {
    INST_CLASS_METHODS;

    TEST_METHOD(BR_branch) {
        inst.Rn = 1;

        SET_REG(GPRegs[1], 0);
        expected.nextPC = 0;
        doTestInst(L"BR branches to register");

        SET_REG(GPRegs[1], 0x1000);
        expected.nextPC = 0x1000;
        doTestInst(L"BR branches to register");

        SET_REG(GPRegs[1], 0x1010101010101010);
        expected.nextPC = 0x1010101010101010;
        doTestInst(L"BR branches to register");
    }

    TEST_METHOD(BR_sp) {
        inst.Rn = 32;
        expected.nextPC = 0;
        doTestInst(L"BR does not use SP");
    }
};

TEST_CLASS(blr), BaseTestClass<cpu::instructionID::BLR> {
    INST_CLASS_METHODS;

    TEST_METHOD(BLR_branch) {
        inst.Rn = 1;

        SET_REG(GPRegs[1], 0);
        expected.nextPC = 0;
        expected.GPRegs[30] = NEXT_PC_DEFAULT;
        doTestInst(L"BLR branches to register and stores PC + 4 in X30");

        SET_REG(PC, 0x2000);
        SET_REG(GPRegs[1], 0x1000);
        expected.nextPC = 0x1000;
        expected.GPRegs[30] = 0x2004;
        doTestInst(L"BLR branches to register and stores PC + 4 in X30");

        SET_REG(PC, 0x3000);
        SET_REG(GPRegs[1], 0x1010101010101010);
        expected.nextPC = 0x1010101010101010;
        expected.GPRegs[30] = 0x3004;
        doTestInst(L"BLR branches to register and stores PC + 4 in X30");
    }

    TEST_METHOD(BLR_sp) {
        inst.Rn = 32;
        expected.nextPC = 0;
        expected.GPRegs[30] = NEXT_PC_DEFAULT;
        doTestInst(L"BLR does not use SP");
    }

    TEST_METHOD(BLR_overflow) {
        SET_REG(GPRegs[1], 0);
        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        inst.Rn = 1;
        expected.nextPC = 0;
        expected.GPRegs[30] = 0;
        doTestInst(L"BLR X30 wraps around on overflow");
    }
};

TEST_CLASS(ret), BaseTestClass<cpu::instructionID::RET> {
    INST_CLASS_METHODS;

    TEST_METHOD(RET_branch) {
        inst.Rn = 1;

        SET_REG(GPRegs[1], 0);
        expected.nextPC = 0;
        doTestInst(L"RET branches to register");

        SET_REG(GPRegs[1], 0x1000);
        expected.nextPC = 0x1000;
        doTestInst(L"RET branches to register");

        SET_REG(GPRegs[1], 0x1010101010101010);
        expected.nextPC = 0x1010101010101010;
        doTestInst(L"RET branches to register");
    }

    TEST_METHOD(RET_sp) {
        inst.Rn = 32;
        expected.nextPC = 0;
        doTestInst(L"RET does not use SP");
    }
};

// ERET is not implemented
// DRPS is not implemented

TEST_CLASS(b), BaseTestClass<cpu::instructionID::B> {
    INST_CLASS_METHODS;

    TEST_METHOD(B_branch) {
        inst._0_25 = 0;
        expected.nextPC = PC_DEFAULT;
        doTestInst(L"B jumps to itself when imm == 0");

        SET_REG(PC, 0x1000);
        inst._0_25 = 2;
        expected.nextPC = 0x1008;
        doTestInst(L"B is in increments of 4");

        SET_REG(PC, 0x1000);
        inst._0_25 = 0x1FFFFFF;
        expected.nextPC = 0x8000FFC;
        doTestInst(L"B is in increments of 4");

        SET_REG(PC, 0x1000);
        inst._0_25 = 0x2000000;
        expected.nextPC = 0xFFFFFFFFF8001000;
        doTestInst(L"B has a signed offset");

        SET_REG(PC, 0x1000);
        inst._0_25 = 0x3FFFFFF;
        expected.nextPC = 0xFFC;
        doTestInst(L"B has a signed offset");
    }

    TEST_METHOD(B_overflow) {
        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        inst._0_25 = 0x1;
        expected.nextPC = 0;
        doTestInst(L"B wraps around on overflow");
    }

    TEST_METHOD(B_underflow) {
        SET_REG(PC, 0);
        inst._0_25 = 0x3FFFFFF;
        expected.nextPC = 0xFFFFFFFFFFFFFFFC;
        doTestInst(L"B wraps around on underflow");
    }
};

TEST_CLASS(bl), BaseTestClass<cpu::instructionID::BL> {
    INST_CLASS_METHODS;

    TEST_METHOD(BL_branch) {
        inst._0_25 = 0;
        expected.nextPC = PC_DEFAULT;
        expected.GPRegs[30] = NEXT_PC_DEFAULT;
        doTestInst(L"BL jumps to itself when imm == 0 and stores PC + 4 in X30");

        SET_REG(PC, 0x1000);
        inst._0_25 = 2;
        expected.nextPC = 0x1008;
        expected.GPRegs[30] = 0x1004;
        doTestInst(L"BL is in increments of 4 and stores PC + 4 in X30");

        SET_REG(PC, 0x1000);
        inst._0_25 = 0x1FFFFFF;
        expected.nextPC = 0x8000FFC;
        expected.GPRegs[30] = 0x1004;
        doTestInst(L"BL is in increments of 4 and stores PC + 4 in X30");

        SET_REG(PC, 0x1000);
        inst._0_25 = 0x2000000;
        expected.nextPC = 0xFFFFFFFFF8001000;
        expected.GPRegs[30] = 0x1004;
        doTestInst(L"BL has a signed offset and stores PC + 4 in X30");

        SET_REG(PC, 0x1000);
        inst._0_25 = 0x3FFFFFF;
        expected.nextPC = 0xFFC;
        expected.GPRegs[30] = 0x1004;
        doTestInst(L"BL has a signed offset and stores PC + 4 in X30");
    }

    TEST_METHOD(BL_overflow) {
        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        inst._0_25 = 0x1;
        expected.nextPC = 0;
        expected.GPRegs[30] = 0;
        doTestInst(L"BL wraps around on overflow and X30 wraps around");
    }

    TEST_METHOD(BL_underflow) {
        SET_REG(PC, 0);
        inst._0_25 = 0x3FFFFFF;
        expected.nextPC = 0xFFFFFFFFFFFFFFFC;
        expected.GPRegs[30] = 4;
        doTestInst(L"BL wraps around on underflow and stores PC + 4 in X30");
    }
};

TEST_CLASS(cbz32), BaseTestClass<cpu::instructionID::CBZ_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(CBZ_32_branch) {
        inst.Rt = 1;

        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0;
        expected.nextPC = PC_DEFAULT;
        doTestInst(L"CBZ_32 jumps to itself when imm == 0");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 2;
        expected.nextPC = 0x1008;
        doTestInst(L"CBZ_32 is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x3FFFF;
        expected.nextPC = 0x100FFC;
        doTestInst(L"CBZ_32 is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x40000;
        expected.nextPC = 0xFFFFFFFFFFF01000;
        doTestInst(L"CBZ_32 has a signed offset");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFC;
        doTestInst(L"CBZ_32 has a signed offset");

        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x2;
        doTestInst(L"CBZ_32 does nothing when the register is not 0");
    }

    TEST_METHOD(CBZ_32_overflow) {
        inst.Rt = 1;

        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x1;
        expected.nextPC = 0;
        doTestInst(L"CBZ_32 wraps around on overflow");
    }

    TEST_METHOD(CBZ_32_underflow) {
        inst.Rt = 1;

        SET_REG(PC, 0);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFFFFFFFFFFFFFFC;
        doTestInst(L"CBZ_32 wraps around on underflow");
    }

    TEST_METHOD(CBZ_32_sp) {
        inst.Rt = 31;

        SET_REG(PC, 0x1000);
        SET_REG(SP, 0x1000);
        inst._5_23 = 0x2;
        expected.nextPC = 0x1008;
        doTestInst(L"CBZ_32 does not use sp");
    }

    TEST_METHOD(CBZ_32_high_bits) {
        inst.Rt = 1;

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0xFFFFFFFF00000000);
        inst._5_23 = 0x2;
        expected.nextPC = 0x1008;
        doTestInst(L"CBZ_32 only uses the low bits");
    }
};

TEST_CLASS(cbnz32), BaseTestClass<cpu::instructionID::CBNZ_32> {
    INST_CLASS_METHODS;

    TEST_METHOD(CBNZ_32_branch) {
        inst.Rt = 1;

        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0;
        expected.nextPC = PC_DEFAULT;
        doTestInst(L"CBNZ_32 jumps to itself when imm == 0");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 2;
        expected.nextPC = 0x1008;
        doTestInst(L"CBNZ_32 is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x3FFFF;
        expected.nextPC = 0x100FFC;
        doTestInst(L"CBNZ_32 is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x40000;
        expected.nextPC = 0xFFFFFFFFFFF01000;
        doTestInst(L"CBNZ_32 has a signed offset");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFC;
        doTestInst(L"CBNZ_32 has a signed offset");

        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x2;
        doTestInst(L"CBNZ_32 does nothing when the register is 0");
    }

    TEST_METHOD(CBNZ_32_overflow) {
        inst.Rt = 1;

        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x1;
        expected.nextPC = 0;
        doTestInst(L"CBNZ_32 wraps around on overflow");
    }

    TEST_METHOD(CBNZ_32_underflow) {
        inst.Rt = 1;

        SET_REG(PC, 0);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFFFFFFFFFFFFFFC;
        doTestInst(L"CBNZ_32 wraps around on underflow");
    }

    TEST_METHOD(CBNZ_32_sp) {
        inst.Rt = 31;

        SET_REG(PC, 0x1000);
        SET_REG(nextPC, 0x1004);
        SET_REG(SP, 0x1000);
        inst._5_23 = 0x2;
        doTestInst(L"CBNZ_32 does not use sp");
    }

    TEST_METHOD(CBNZ_32_high_bits) {
        inst.Rt = 1;

        SET_REG(PC, 0x1000);
        SET_REG(nextPC, 0x1004);
        SET_REG(GPRegs[1], 0xFFFFFFFF00000000);
        inst._5_23 = 0x2;
        doTestInst(L"CBNZ_32 only uses the low bits");
    }
};

TEST_CLASS(cbz64), BaseTestClass<cpu::instructionID::CBZ_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(CBZ_64_branch) {
        inst.Rt = 1;

        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0;
        expected.nextPC = PC_DEFAULT;
        doTestInst(L"CBZ_64 jumps to itself when imm == 0");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 2;
        expected.nextPC = 0x1008;
        doTestInst(L"CBZ_64 is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x3FFFF;
        expected.nextPC = 0x100FFC;
        doTestInst(L"CBZ_64 is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x40000;
        expected.nextPC = 0xFFFFFFFFFFF01000;
        doTestInst(L"CBZ_64 has a signed offset");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFC;
        doTestInst(L"CBZ_64 has a signed offset");

        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x2;
        doTestInst(L"CBZ_64 does nothing when the register is not 0");
    }

    TEST_METHOD(CBZ_64_overflow) {
        inst.Rt = 1;

        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x1;
        expected.nextPC = 0;
        doTestInst(L"CBZ_64 wraps around on overflow");
    }

    TEST_METHOD(CBZ_64_underflow) {
        inst.Rt = 1;

        SET_REG(PC, 0);
        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFFFFFFFFFFFFFFC;
        doTestInst(L"CBZ_64 wraps around on underflow");
    }

    TEST_METHOD(CBZ_64_sp) {
        inst.Rt = 31;

        SET_REG(PC, 0x1000);
        SET_REG(SP, 0x1000);
        inst._5_23 = 0x2;
        expected.nextPC = 0x1008;
        doTestInst(L"CBZ_64 does not use sp");
    }

    TEST_METHOD(CBZ_64_high_bits) {
        inst.Rt = 1;

        SET_REG(PC, 0x1000);
        SET_REG(nextPC, 0x1004);
        SET_REG(GPRegs[1], 0xFFFFFFFF00000000);
        inst._5_23 = 0x2;
        doTestInst(L"CBZ_64 does use the high bits");
    }
};

TEST_CLASS(cbnz64), BaseTestClass<cpu::instructionID::CBNZ_64> {
    INST_CLASS_METHODS;

    TEST_METHOD(CBNZ_64_branch) {
        inst.Rt = 1;

        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0;
        expected.nextPC = PC_DEFAULT;
        doTestInst(L"CBNZ_64 jumps to itself when imm == 0");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 2;
        expected.nextPC = 0x1008;
        doTestInst(L"CBNZ_64 is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x3FFFF;
        expected.nextPC = 0x100FFC;
        doTestInst(L"CBNZ_64 is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x40000;
        expected.nextPC = 0xFFFFFFFFFFF01000;
        doTestInst(L"CBNZ_64 has a signed offset");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFC;
        doTestInst(L"CBNZ_64 has a signed offset");

        SET_REG(GPRegs[1], 0);
        inst._5_23 = 0x2;
        doTestInst(L"CBNZ_64 does nothing when the register is 0");
    }

    TEST_METHOD(CBNZ_64_overflow) {
        inst.Rt = 1;

        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x1;
        expected.nextPC = 0;
        doTestInst(L"CBNZ_64 wraps around on overflow");
    }

    TEST_METHOD(CBNZ_64_underflow) {
        inst.Rt = 1;

        SET_REG(PC, 0);
        SET_REG(GPRegs[1], 1);
        inst._5_23 = 0x7FFFF;
        expected.nextPC = 0xFFFFFFFFFFFFFFFC;
        doTestInst(L"CBNZ_64 wraps around on underflow");
    }

    TEST_METHOD(CBNZ_64_sp) {
        inst.Rt = 31;

        SET_REG(PC, 0x1000);
        SET_REG(nextPC, 0x1004);
        SET_REG(SP, 0x1000);
        inst._5_23 = 0x2;
        doTestInst(L"CBNZ_64 does not use sp");
    }

    TEST_METHOD(CBNZ_64_high_bits) {
        inst.Rt = 1;

        SET_REG(PC, 0x1000);
        SET_REG(nextPC, 0x1004);
        SET_REG(GPRegs[1], 0xFFFFFFFF00000000);
        inst._5_23 = 0x2;
        expected.nextPC = 0x1008;
        doTestInst(L"CBNZ_64 does use high bits");
    }
};

TEST_CLASS(tbz), BaseTestClass<cpu::instructionID::TBZ> {
    INST_CLASS_METHODS;

    TEST_METHOD(TBZ_branch) {
        inst.Rt = 1;

        SET_REG(GPRegs[1], 0);
        inst._5_18 = 0;
        expected.nextPC = PC_DEFAULT;
        doTestInst(L"TBZ jumps to itself when imm == 0");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_18 = 2;
        expected.nextPC = 0x1008;
        doTestInst(L"TBZ is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_18 = 0x1FFF;
        expected.nextPC = 0x8FFC;
        doTestInst(L"TBZ is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_18 = 0x2000;
        expected.nextPC = 0xFFFFFFFFFFFF9000;
        doTestInst(L"TBZ has a signed offset");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0);
        inst._5_18 = 0x3FFF;
        expected.nextPC = 0xFFC;
        doTestInst(L"TBZ has a signed offset");

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._5_18 = 0x1;
        doTestInst(L"TBZ does nothing when the condition is not met");
    }

    TEST_METHOD(TBZ_overflow) {
        inst.Rt = 1;

        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        SET_REG(GPRegs[1], 0);
        inst._5_18 = 0x1;
        expected.nextPC = 0;
        doTestInst(L"TBZ wraps around on overflow");
    }

    TEST_METHOD(TBZ_underflow) {
        inst.Rt = 1;

        SET_REG(PC, 0);
        SET_REG(GPRegs[1], 0);
        inst._5_18 = 0x3FFF;
        expected.nextPC = 0xFFFFFFFFFFFFFFFC;
        doTestInst(L"TBZ wraps around on underflow");
    }

    TEST_METHOD(TBZ_all) {
        inst.Rt = 1;

        inst._5_18 = 0x2;

        for (uint64_t index = 0; index < 64; index++) {
            for (uint64_t bit = 0; bit < 64; bit++) {
                inst.b40 = bit & 0x1F;
                inst.b5 = bit >> 5;

                SET_REG(PC, 0x1000);
                SET_REG(nextPC, 0x1004);
                SET_REG(GPRegs[1], setBitTable[index]);

                if (bit == index) {
                    doTestInst(L"TBZ does not branch on a set bit");
                } else {
                    expected.nextPC = 0x1008;
                    doTestInst(L"TBZ does branch on a clear bit");
                }

                SET_REG(PC, 0x1000);
                SET_REG(nextPC, 0x1004);
                SET_REG(GPRegs[1], clearBitTable[index]);

                if (bit != index) {
                    doTestInst(L"TBZ does not branch on a set bit");
                } else {
                    expected.nextPC = 0x1008;
                    doTestInst(L"TBZ does branch on a clear bit");
                }
            }
        }
    }
};

TEST_CLASS(tbnz), BaseTestClass<cpu::instructionID::TBNZ> {
    INST_CLASS_METHODS;

    TEST_METHOD(TBNZ_branch) {
        inst.Rt = 1;

        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._5_18 = 0;
        expected.nextPC = PC_DEFAULT;
        doTestInst(L"TBNZ jumps to itself when imm == 0");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._5_18 = 2;
        expected.nextPC = 0x1008;
        doTestInst(L"TBNZ is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._5_18 = 0x1FFF;
        expected.nextPC = 0x8FFC;
        doTestInst(L"TBNZ is in increments of 4");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._5_18 = 0x2000;
        expected.nextPC = 0xFFFFFFFFFFFF9000;
        doTestInst(L"TBNZ has a signed offset");

        SET_REG(PC, 0x1000);
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._5_18 = 0x3FFF;
        expected.nextPC = 0xFFC;
        doTestInst(L"TBNZ has a signed offset");

        SET_REG(GPRegs[1], 0);
        inst._5_18 = 0x1;
        doTestInst(L"TBNZ does nothing when the condition is not met");
    }

    TEST_METHOD(TBNZ_overflow) {
        inst.Rt = 1;

        SET_REG(PC, 0xFFFFFFFFFFFFFFFC);
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._5_18 = 0x1;
        expected.nextPC = 0;
        doTestInst(L"TBNZ wraps around on overflow");
    }

    TEST_METHOD(TBNZ_underflow) {
        inst.Rt = 1;

        SET_REG(PC, 0);
        SET_REG(GPRegs[1], 0xFFFFFFFFFFFFFFFF);
        inst._5_18 = 0x3FFF;
        expected.nextPC = 0xFFFFFFFFFFFFFFFC;
        doTestInst(L"TBNZ wraps around on underflow");
    }

    TEST_METHOD(TBNZ_all) {
        inst.Rt = 1;

        inst._5_18 = 0x2;

        for (uint64_t index = 0; index < 64; index++) {
            for (uint64_t bit = 0; bit < 64; bit++) {
                inst.b40 = bit & 0x1F;
                inst.b5 = bit >> 5;

                SET_REG(PC, 0x1000);
                SET_REG(nextPC, 0x1004);
                SET_REG(GPRegs[1], setBitTable[index]);

                if (bit != index) {
                    doTestInst(L"TBNZ does branch on a set bit");
                } else {
                    expected.nextPC = 0x1008;
                    doTestInst(L"TBNZ does not branch on a clear bit");
                }

                SET_REG(PC, 0x1000);
                SET_REG(nextPC, 0x1004);
                SET_REG(GPRegs[1], clearBitTable[index]);

                if (bit == index) {
                    doTestInst(L"TBNZ does branch on a clear bit");
                } else {
                    expected.nextPC = 0x1008;
                    doTestInst(L"TBNZ does not branch on a set bit");
                }
            }
        }
    }
};

} // namespace InterpreterUnitTest