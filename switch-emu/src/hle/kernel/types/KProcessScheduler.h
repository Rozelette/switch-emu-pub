#pragma once
#include <stdint.h>

#include "structutils.h"

struct KProcessScheduler {
    UNKNOWN(0x10);
};
CHECK_SIZE(KProcessScheduler, 0x10);
