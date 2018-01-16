#include <string>
#include <vector>
#include <thread>
#include <array>
#include <unordered_set>

#include <condition_variable>
#include <mutex>

#include "bitutils.h"
#include "cpu.h"
#include "float.h"
#include "mem.h"
#include "log.h"
#include "unreachable.h"
#include "decode.h"
#include "disassembler.h"
#include "interpreter\interpreter.h"
#include "jit\jit.h"
#include "platform_thread.h"

namespace cpu {

thread_local Core* tCurrentCore = nullptr;

std::array<Core, 4> gCores;
bool gRunning = true;
bool gStartPaused = false; // TODO get this working again
bool gIsJitEnabled = false;

std::mutex gInterruptLock;
std::condition_variable gInterruptCondVar;

std::unordered_set<uint64_t> gBreakpoints;
std::unordered_set<uint64_t> gSingleUseBreakpoints;
bool gSingleStepMode = false;

EntryFunc gEntryPoint;
InterruptHandler gInterruptHandler;
SegFaultHandler gSegFaultHandler;
IllInstHandler gIllInstHandler;
SVCHandler gSVCHandler;

void printCoreState(Core* core) {
    auto state = core->PSTATE;
    for (int i = 0; i < 16; i++) {
        logger::info("GPR%02d:%016llX GPR%02d:%016llX", i, core->GPRegs[i], i + 16, core->GPRegs[i + 16]);
    }
    for (int i = 0; i < 32; i++) {
        logger::info("VReg%02d:%016llX%016llX S:%f D:%f", i, core->VRegs[i].D[1], core->VRegs[i].D[0], core->VRegs[i].floatS.val, core->VRegs[i].floatD.val);
    }
    logger::info("PC:   %016llX ", core->PC);
    logger::info("SP:   %016llX", core->SP);
    logger::info("N:%s Z:%s C:%s V:%s",
        (state.N)? "1" : "0",
        (state.Z)? "1" : "0",
        (state.C)? "1" : "0",
        (state.V)? "1" : "0");
}

void init() {
    decoder::init();
    interpreter::init();
    jit::init();

    disassembler::init();

    //addBreakpoint(0x28045658);
    //addBreakpoint(0x2804b908);
    //addBreakpoint(0x2804c934);
    //addBreakpoint(0x2804b34c);
    //addBreakpoint(0x280ab8f8);
    //addBreakpoint(0x280a6510);
    //addBreakpoint(0x280a6498);
    //addBreakpoint(0x280ab5cc);

    //addBreakpoint(0x280a6508);

    //addBreakpoint(0x10002538);
    //addBreakpoint(0x100019f8);
    //addBreakpoint(0x10001850);
    //addBreakpoint(0x100023c0);

    //addBreakpoint(0x8000008);
    //addBreakpoint(0x280AB5CC);
    //addBreakpoint(0x280A6498);
    //addBreakpoint(0x280A6508);

    //addBreakpoint(0x200061ac);

    //addBreakpoint(0x280b3244);
    //addBreakpoint(0x280c40b8);
    //addBreakpoint(0x280165cc);
    //addBreakpoint(0x28015c14);
    //addBreakpoint(0x28015d28);

    //addBreakpoint(0x100e9bf4);

    //addBreakpoint(0x10019620);
    //addBreakpoint(0x100198B0);
    //addBreakpoint(0x18088824);
    //addBreakpoint(0x180c4e88);
    //addBreakpoint(0x180c50a0);
    //addBreakpoint(0x180c99c8);
}

void start() {
    for (int i = 0; i < NUM_CORES; i++) {
        auto& core = gCores[i];
        core.coreNum = i;

        core.thread = std::thread([&core, i]() {
            tCurrentCore = &core;
            tCurrentCore->isActive = false;

            gEntryPoint();
        });
    }

    platform::setThreadName(&gCores[0].thread, "Core 0");
    platform::setThreadName(&gCores[1].thread, "Core 1");
    platform::setThreadName(&gCores[2].thread, "Core 2");
    platform::setThreadName(&gCores[3].thread, "Core 3");
}

Core* getCore(uint64_t i) {
    return &gCores[i];
}

void halt() {
    for (uint64_t core = 0; core < NUM_CORES; core++) {
        sendInterrupts(core, RESET_INTERRUPT);
    }
}

void join() {
    for (auto& core :gCores) {
        if (core.thread.joinable()) {
            core.thread.join();
        }
    }
}

void step(uint64_t core) {
    if (gIsJitEnabled) {
        cpu::jit::step(&gCores[core]);
    } else {
        cpu::interpreter::step(&gCores[core]);
    }
}

void setEntryPoint(EntryFunc func) {
    gEntryPoint = func;
}

void setInterruptHandler(InterruptHandler func) {
    gInterruptHandler = func;
}

void setSegFaultHandler(SegFaultHandler func) {
    gSegFaultHandler = func;
}

void setIllInstHandler(IllInstHandler func) {
    gIllInstHandler = func;
}

void setSVCHandler(SVCHandler func) {
    gSVCHandler = func;
}

void addBreakpoint(uint64_t addr) {
    gBreakpoints.insert(addr);
}

void addSingleUseBreakpoint(uint64_t addr) {
    gSingleUseBreakpoints.insert(addr);
}

void removeBreakpoint(uint64_t addr) {
    gBreakpoints.erase(addr);
}

void setSingleStepMode(bool isEnabled) {
    gSingleStepMode = isEnabled;
}

void sendInterrupts(uint64_t core, uint64_t interrupts) {
    //logger::info("Sending interrupt to core %d", core);
    std::unique_lock<std::mutex> lock(gInterruptLock);
    gCores[core].interrupts.fetch_or(interrupts);
    gInterruptCondVar.notify_all();
}

namespace this_core {

void resume() {
    while (tCurrentCore->isActive) {
        checkInterrupts();
        if (gIsJitEnabled) {
            cpu::jit::step(tCurrentCore);
        } else {
            cpu::interpreter::step(tCurrentCore);
        }

        // TODO this has several problems:
        // -invisible to jit
        // -slow
        // potential solution: swap out instructions at breakpoints with traps?
        if (gBreakpoints.count(tCurrentCore->PC) > 0) {
            logger::warn("Breakpoint hit");
            cpu::sendInterrupts(cpu::this_core::getCoreID(), cpu::DEBUG_BREAK_INTERRUPT);
        }
        if (gSingleUseBreakpoints.count(tCurrentCore->PC) > 0) {
            cpu::sendInterrupts(cpu::this_core::getCoreID(), cpu::DEBUG_BREAK_INTERRUPT);
            gSingleUseBreakpoints.erase(tCurrentCore->PC);
        }
        if (gSingleStepMode) {
            cpu::sendInterrupts(cpu::this_core::getCoreID(), cpu::DEBUG_BREAK_INTERRUPT);
        }
    }
}

int32_t getCoreID() {
    return tCurrentCore->coreNum;
}

Core* getCore() {
    return tCurrentCore;
}

void clearInterrupts(uint64_t interrupts) {
    tCurrentCore->interrupts.fetch_and(~interrupts);
}

void checkInterrupts() {
    uint64_t interrupts = tCurrentCore->interrupts.fetch_and(0);
    if (interrupts) {
        gInterruptHandler(interrupts);
    }
}

void waitForInterrupt() {
    std::unique_lock<std::mutex> lock(gInterruptLock); // lock now so no interrupts come between checking and waiting

    while (true) {
        uint64_t interrupts = tCurrentCore->interrupts.fetch_and(0);
        if (interrupts) {
            lock.unlock();
            gInterruptHandler(interrupts);
            return;
        } else {
            gInterruptCondVar.wait(lock);
        }
    }
}

} // namespace this_core

} // namespace cpu