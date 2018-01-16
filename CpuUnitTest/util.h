#pragma once
#define INST_CLASS_METHODS \
public: \
    TEST_CLASS_INITIALIZE(methodName) {\
        cpu::interpreter::init(); \
    } \
    TEST_METHOD_INITIALIZE(init) { \
        BaseTestClass::init(); \
    } \
    TEST_METHOD_CLEANUP(fini) { \
        BaseTestClass::fini(); \
    }

#define SET_REG(reg, val) \
core.reg = val; \
expected.reg = val; \
jitCore.reg = val;
