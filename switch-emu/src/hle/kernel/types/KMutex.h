#pragma once
#include <stdint.h>

#include "structutils.h"

// Assumptions:
//  - Does not block if the lock is not gotten (that's what KSpinLock is for).
//    Further evidenced because this does not track which threads are waiting for it.
//  - 0 is an invalid owner.
//  - ownerTag is the threadId of the thread that has the lock.
//  - Not reentrant, so tryLock(ownerTag) -> false (that's what KRecursiveLock is for).
// Thoughts:
//  - Threads that get destroyed should probably release all the locks they hold. Where is this tracked?
//    KThread has some unknown lists in it. Could that be it? KMutex could lookup a thread by the
//    ownerTag and inform it has acquired the lock.
class KMutex {
    static constexpr uint64_t NO_OWNER = 0;

public:
    constexpr KMutex() : ownerTag(NO_OWNER) {}
    KMutex(const KMutex&) = delete;

    KMutex& operator= (const KMutex&) = delete;

    bool tryLock(uint64_t source);
    void unlock(uint64_t source);

private:
    uint64_t ownerTag = NO_OWNER;
};
CHECK_SIZE(KMutex, 0x8);
