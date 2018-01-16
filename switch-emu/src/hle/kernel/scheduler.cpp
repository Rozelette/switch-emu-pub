#include <array>
#include <mutex>
#include <list>

#include "scheduler.h"
#include "cpu.h"
#include "log.h"
#include "time.h"
#include "alarm.h"

namespace kernel::scheduler {

// TODO asserts and other error checking

// TODO should use tls pages from KProcess
uint64_t gTlsSlotcounter = 1;

std::mutex gSchedulerLock;
std::array<handle_t, cpu::NUM_CORES> gCurrentThreads = { INVALID_THREAD, INVALID_THREAD, INVALID_THREAD, INVALID_THREAD };
std::array<std::list<handle_t>, cpu::NUM_CORES> gThreadLists; // TODO keep these sorted? Or have a seperate list for each priority?

handle_t getCurrentHandle() {
    return gCurrentThreads[cpu::this_core::getCoreID()];
}

ObjectPtr<Thread> getCurrentThread() {
    return getThread(getCurrentHandle());
}

ObjectPtr<Thread> getThread(handle_t handle) {
    if (handle == INVALID_THREAD) {
        return ObjectPtr<Thread>(nullptr); // TODO shouldn't need wrapper?
    }
    return kernel::object_pointer_cast<Thread>(kernel::handleman::lookupObject(handle));
}

std::list<handle_t>& getCurrentThreadList() {
    return gThreadLists[cpu::this_core::getCoreID()];
}

void startThread(handle_t handle) {
    logger::info("Starting thread with handle %d", handle);

    ObjectPtr<Thread> thread = getThread(handle);
    thread->status = ThreadStatus::READY;

    lockScheduler();
    gThreadLists[static_cast<int>(thread->affinity)].push_back(handle);
    unlockScheduler();
}

handle_t createThread(uint64_t entry, uint64_t arg, uint64_t stack, uint32_t priority, CoreAffinity cores) {
    lockScheduler();
    Thread* thread = new Thread(entry, arg, stack, priority, cores);
    thread->status = ThreadStatus::INITIALIZED;
    thread->context.TLSAddr = 0x700000000 + (gTlsSlotcounter++) * 0x200; // TODO actually use TLS slots
    handle_t assignedHandle = kernel::handleman::createHandle(ObjectPtr<Thread>(thread));
    unlockScheduler();

    logger::info("Created thread with handle 0x%X", assignedHandle);

    return assignedHandle;
}

void yieldThread() {
    lockScheduler();
    logger::info("Yielding thread with handle %d", getCurrentHandle());

    queueThreadNoLock(getCurrentHandle());
    rescheduleNoLock();
    unlockScheduler();
}

void sleepThread(uint64_t nano) {
    auto& alarm = kernel::alarm::createAlarm(kernel::time::getTimeInNanoseconds() + nano);

    alarm.wait(kernel::scheduler::getCurrentHandle());
}

// TODO how to handle ties
handle_t popBest(std::list<handle_t>& list, uint64_t minPriority) {
    std::list<handle_t>::iterator best = list.end();
    uint64_t bestPriority = minPriority;

    for (auto handle = list.begin(); handle != list.end(); handle++) {
        if (*handle != INVALID_THREAD) {
            if (getThread(*handle)->priority < bestPriority) {
                bestPriority = getThread(*handle)->priority;
                best = handle;
            }
        }
    }

    if (best != list.end()) {
        handle_t ret = *best;
        list.erase(best);
        return ret;
    } else {
        return INVALID_THREAD;
    }
}

void rescheduleNoLock() {
    int32_t currentCoreNum = cpu::this_core::getCoreID();
    std::list<handle_t>& currentList = getCurrentThreadList();
    cpu::Core* core = cpu::this_core::getCore();
    handle_t currentHandle = getCurrentHandle();
    ObjectPtr<Thread> currentThread = getCurrentThread();
    uint64_t currentPriority = (currentThread.get() != nullptr) ? currentThread->priority : NO_PRIORITY;

    handle_t nextHandle = popBest(currentList, currentPriority);

    if ((nextHandle == INVALID_THREAD) && (currentThread.get() != nullptr) && (currentThread->status == ThreadStatus::RUNNING)) {
        nextHandle = currentHandle;
    }

    if (currentHandle != INVALID_THREAD) {
        if (nextHandle != INVALID_THREAD) {
            logger::info("Reschedule from %d to %d on core %d with %d waiting threads", currentHandle, nextHandle, currentCoreNum, currentList.size());

            if (currentHandle != nextHandle) {
                kernel::contextSwitch(getThread(nextHandle));
                getThread(nextHandle)->status = ThreadStatus::RUNNING;

                if (currentThread->status == ThreadStatus::RUNNING) {
                    currentThread->status = ThreadStatus::READY;
                    queueThreadNoLock(currentHandle);
                }
            }
            core->isActive = true;
        } else {
            logger::info("Reschedule from %d to idle on core %d with %d waiting threads", currentHandle, currentCoreNum, currentList.size());

            kernel::contextSwitch(ObjectPtr<Thread>(nullptr));
            if (currentThread->status == ThreadStatus::RUNNING) {
                currentThread->status = ThreadStatus::READY;
                queueThreadNoLock(currentHandle);
            }
            core->isActive = false;
        }
    } else {
        if (nextHandle != INVALID_THREAD) {
            logger::info("Reschedule from idle to %d on core %d with %d waiting threads", nextHandle, currentCoreNum, currentList.size());

            kernel::contextSwitch(getThread(nextHandle));
            getThread(nextHandle)->status = ThreadStatus::RUNNING;
            core->isActive = true;
        } else {
            logger::info("Reschedule from idle to idle on core %d with %d waiting threads", currentCoreNum, currentList.size());

            core->isActive = false;
        }
    }
    gCurrentThreads[currentCoreNum] = nextHandle;
    if (nextHandle != INVALID_THREAD) {
        getThread(nextHandle)->wakeup();
    }
}

void queueThreadNoLock(handle_t handle) {
    ObjectPtr<Thread> thread = getThread(handle);
    gThreadLists[static_cast<int>(thread->affinity)].push_back(handle);
    thread->status = ThreadStatus::READY;
}

void exitThread() {
    // TODO thread clean-up
    lockScheduler();
    handle_t thisHandle = getCurrentHandle();
    getCurrentThread()->status = ThreadStatus::UNINITIATED;
    rescheduleNoLock();
    unlockScheduler();

    logger::info("Exited thread with handle %d", thisHandle);
}

void lockScheduler() {
    gSchedulerLock.lock();
}

void unlockScheduler() {
    gSchedulerLock.unlock();
}

} // namespace kernel::scheduler
