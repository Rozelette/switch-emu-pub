#pragma once
#include <stdint.h>

#include "structutils.h"

struct KSpinLock {
    UNKNOWN(0x40);
    // TODO
};
CHECK_SIZE(KSpinLock, 0x40); // ???
