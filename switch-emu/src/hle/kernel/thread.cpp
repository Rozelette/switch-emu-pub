#include "thread.h"
#include "cpu.h"
#include "scheduler.h"
#include "log.h"

namespace kernel {

uint64_t Thread::getTLS() {
    return context.TLSAddr;
}

// TODO remove?
Thread::Thread() {
    for (int i = 0; i < 31; i++) {
        context.GPRegs[i] = 0;
    }
    for (int i = 0; i < 32; i++) {
        context.VRegs[i].D[0] = 0;
        context.VRegs[i].D[1] = 0;
    }

    context.PC = 0xDEADBEEFDEADBEEF;
    context.SP = 0xDEADBEEFDEADBEEF;
    context.TLSAddr = 0xDEADBEEFDEADBEEF;

    status = ThreadStatus::UNINITIATED;
    affinity = CoreAffinity::CORE0;

    wakeupCallback = DEFAULT_WAKEUP_CALLBACK;
}

Thread::Thread(uint64_t entry, uint64_t arg, uint64_t stack, uint32_t priorityIn, CoreAffinity affinityIn) {
    for (int i = 0; i < 31; i++) {
        context.GPRegs[i] = 0;
    }
    for (int i = 0; i < 32; i++) {
        context.VRegs[i].D[0] = 0;
        context.VRegs[i].D[1] = 0;
    }

    context.PC = entry;
    context.GPRegs[0] = arg; // TODO is this correct?
    context.SP = stack;
    priority = priorityIn;
    context.TLSAddr = 0; // TODO input parameter?
    context.GPRegs[30] = 0x0BADC0DE;

    status = ThreadStatus::UNINITIATED;
    if (affinityIn == CoreAffinity::DEFAULT_CORE) {
        // TODO properly use program default core
        affinity = CoreAffinity::CORE0;
    } else {
        affinity = affinityIn;
    }

    wakeupCallback = DEFAULT_WAKEUP_CALLBACK;
}

// TODO pass in what it's waiting on
void Thread::wait() {
    status = ThreadStatus::WAITING;
}

void Thread::wakeup() {
    wakeupCallback(ObjectPtr<Thread>(this));
    wakeupCallback = DEFAULT_WAKEUP_CALLBACK;
}

void Thread::exit() {
    status = ThreadStatus::EXITED;
    for (auto& handle : waitingThreads) {
        scheduler::queueThreadNoLock(handle);
    }
    waitingThreads.clear();
}

void Thread::sync(handle_t handle) {
    if (status == ThreadStatus::EXITED) {
        scheduler::queueThreadNoLock(handle);
    } else {
        waitingThreads.push_back(handle);
        scheduler::getThread(handle)->wait();
    }
    cpu::sendInterrupts(0, cpu::GENERIC_INTERRUPT);
}

void Thread::cancelAllWaitObjects() {
    // TODO
}

void Thread::removeHandle(handle_t handle) {
    for (auto iter = waitingThreads.begin(); iter != waitingThreads.end(); iter++) {
        if (*iter == handle) {
            waitingThreads.erase(iter);
            return;
        }
    }
    // TODO assert?
}

void saveState(ThreadContext* context) {
    cpu::Core* state = cpu::this_core::getCore();

    for (int i = 0; i < 31; i++) {
        context->GPRegs[i] = state->GPRegs[i];
    }
    for (int i = 0; i < 32; i++) {
        // TODO better way to copy value?
        context->VRegs[i].D[0] = state->VRegs[i].D[0];
        context->VRegs[i].D[1] = state->VRegs[i].D[1];
    }
    context->PC = state->PC;
    context->SP = state->SP;
    context->PSTATE = state->PSTATE;
    context->TLSAddr = state->TLSAddr;
}

void restoreState(ThreadContext* context) {
    cpu::Core* state = cpu::this_core::getCore();

    for (int i = 0; i < 31; i++) {
        state->GPRegs[i] = context->GPRegs[i];
    }
    for (int i = 0; i < 32; i++) {
        // TODO better way to copy value?
        state->VRegs[i].D[0] = context->VRegs[i].D[0];
        state->VRegs[i].D[1] = context->VRegs[i].D[1];
    }
    state->PC = context->PC;
    state->SP = context->SP;
    state->PSTATE = context->PSTATE;
    state->TLSAddr = context->TLSAddr;
}

void contextSwitch(ObjectPtr<Thread> newThread) {
    logger::info("Context switch");

    ObjectPtr<Thread> oldThread = kernel::scheduler::getCurrentThread();

    if (oldThread) {
        saveState(&oldThread->context);
    }

    if (newThread) {
        restoreState(&newThread->context);
    }

    cpu::this_core::getCore()->exclusiveAccess = false;
}

}