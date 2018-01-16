#pragma once
#include <stdint.h>
#include <string>
#include <array>
#include <atomic>

#include "types.h"

namespace cpu {

const uint64_t GENERIC_INTERRUPT = 1 << 0;
const uint64_t DEBUG_BREAK_INTERRUPT = 1 << 1;
const uint64_t RESET_INTERRUPT = 1 << 2;

// TODO seperate state struct
struct Core {
    bool isActive;
    std::thread thread;
    int coreNum;

    uint64_t GPRegs[32]; // 32nd register does not exist, but we will use it for a zero register
    VReg VRegs[32];
    uint64_t PC;
    uint64_t SP;

    PSTATE_t PSTATE;

    uint64_t TLSAddr;

    uint64_t nextPC;
    uint64_t numInsts;

    bool exclusiveAccess;
    uint64_t exclusiveAccessAddr;
    // TODO 128b int?
    uint64_t exclusiveData0;
    uint64_t exclusiveData1;

    std::atomic<uint64_t> interrupts;
};

const int NUM_CORES = 4;

void init();
void start();
void halt();
void join();
void step(uint64_t core);

using EntryFunc = void(*)();
using InterruptHandler = void(*)(uint64_t interrupts);
using SegFaultHandler = void(*)(uint64_t addr);
using IllInstHandler = void(*)();
using SVCHandler = void(*)(uint64_t svcNum);

void setEntryPoint(EntryFunc func);
void setInterruptHandler(InterruptHandler func);
void setSegFaultHandler(SegFaultHandler func);
void setIllInstHandler(IllInstHandler func);
void setSVCHandler(SVCHandler func);
// TODO hvc, etc.

void sendInterrupts(uint64_t core, uint64_t interrupts);

Core* getCore(uint64_t i);
void addBreakpoint(uint64_t addr);
void addSingleUseBreakpoint(uint64_t addr);
void removeBreakpoint(uint64_t addr);
void setSingleStepMode(bool isEnabled);

namespace this_core {

void resume();

int32_t getCoreID();
Core* getCore(); // TODO try to avoid this

void clearInterrupts(uint64_t interrupts);
void checkInterrupts();
void waitForInterrupt();

} // namespace this_core

} // namespace cpu