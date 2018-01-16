#pragma once
#include "thread.h"
#include "handleManager.h"

namespace kernel::scheduler {

// TODO use general handle manager?
const handle_t INVALID_THREAD = 0;

handle_t getCurrentHandle();
ObjectPtr<Thread> getCurrentThread();
ObjectPtr<Thread> getThread(handle_t handle);

handle_t createThread(uint64_t entry, uint64_t arg, uint64_t stack, uint32_t priority, CoreAffinity cores);
void startThread(handle_t handle);
void exitThread();
void yieldThread();
void sleepThread(uint64_t nano);

void rescheduleNoLock();
void queueThreadNoLock(handle_t handle);

void lockScheduler();
void unlockScheduler();

} // namespace kernel::scheduler
