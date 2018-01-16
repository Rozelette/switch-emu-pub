#pragma once
#include <stdint.h>

#include "structutils.h"

class KThread;

struct KRecursiveLock {
    KThread* owner;
    int32_t count;
    UNKNOWN(0x8);
};
CHECK_OFFSET(KRecursiveLock, owner, 0);
CHECK_OFFSET(KRecursiveLock, count, 0x8);
CHECK_SIZE(KRecursiveLock, 0x18);
