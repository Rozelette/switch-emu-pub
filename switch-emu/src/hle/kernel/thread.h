#pragma once
#include <stdint.h>
#include <string.h>

#include "types.h"
#include "hle\kernel\types\KThread.h"
#include "handleManager.h"

namespace kernel {

// TODO is there no ALL_CORES?
enum class CoreAffinity : uint32_t {
    CORE0 = 0,
    CORE1 = 1,
    CORE2 = 2,
    CORE3 = 3,
    DEFAULT_CORE = 0xFFFFFFFE
};

// TODO are these the actual values?
const uint64_t HIGHEST_PRIORITY = 0;
const uint64_t LOWEST_PRIORITY = 63;
const uint64_t NO_PRIORITY = 64;

enum class ThreadStatus {
    UNINITIATED,
    INITIALIZED,
    READY,
    RUNNING,
    WAITING,
    EXITED
};

// TODO other regs, at least FPSR and FPCR
struct ThreadContext {
    uint64_t GPRegs[31];
    VReg VRegs[32];
    uint64_t PC;
    uint64_t SP;
    PSTATE_t PSTATE;
    uint64_t TLSAddr;
};

// TODO should probably use KThread for this
class Thread : public KThread {
public:
    ThreadContext context;
    std::string name;
    ThreadStatus status = ThreadStatus::UNINITIATED;
    CoreAffinity affinity;
    uint64_t priority;
    std::vector<handle_t> waitingThreads;

    using WakeupCallbackFunc = void(ObjectPtr<Thread>);
    std::function<WakeupCallbackFunc> wakeupCallback;

    WakeupCallbackFunc* DEFAULT_WAKEUP_CALLBACK = [](ObjectPtr<Thread>) {};

    Thread();

    Thread(uint64_t entry, uint64_t arg, uint64_t stack, uint32_t priorityIn, CoreAffinity affinityIn);

    uint64_t getTLS();

    void wait();
    void wakeup();

    void exit();

    virtual void sync(handle_t handle);
    virtual void removeHandle(handle_t handle);
    void cancelAllWaitObjects();
};

void contextSwitch(ObjectPtr<Thread> newThread);
void saveState(ThreadContext* context);
void restoreState(ThreadContext* context);

} // namespace kernel
