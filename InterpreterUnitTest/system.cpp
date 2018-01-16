#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace InterpreterUnitTest {

// No need to test exception generating instrutions (except maybe svc, hvc, smc? just to see if the handler is called)
// No need to test nop (hint, yield, etc.)

// TODO MSR/MRS
// TODO DSB/DMB/ISB (nop?)

TEST_CLASS(clrex), BaseTestClass<cpu::instructionID::CLREX> {
    INST_CLASS_METHODS;

    TEST_METHOD(CLREX_basic) {
        SET_REG(exclusiveAccess, false);
        doTestInst(L"CLREX does nothing when core does not have exclusive access");

        SET_REG(exclusiveAccess, true);
        expected.exclusiveAccess = false;
        doTestInst(L"CLREX clears exclusive access");
    }
};

} // namespace InterpreterUnitTest