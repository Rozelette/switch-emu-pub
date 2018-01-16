#pragma once
#include <stdint.h>

#include "structutils.h"

struct KPageTable {
    void* rawPageTablePointer;
    UNKNOWN(0x1);
    uint32_t addrSpaceSizeInGb;
};
CHECK_OFFSET(KPageTable, rawPageTablePointer, 0x0);
CHECK_OFFSET(KPageTable, addrSpaceSizeInGb, 0xC);
CHECK_SIZE(KPageTable, 0x10);
