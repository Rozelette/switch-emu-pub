#pragma once
#include <stdint.h>

#include "structutils.h"

struct KMemoryBlockList {
    UNKNOWN(0x18);
};
CHECK_SIZE(KMemoryBlockList, 0x18); // ???
