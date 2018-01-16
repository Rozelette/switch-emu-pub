#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "structutils.h"

struct KHandleEntry {
    uint16_t handleId;
    ObjectType objectType;
    uint32_t objectCompressedPointer;
};
CHECK_OFFSET(KHandleEntry, handleId, 0x0);
CHECK_OFFSET(KHandleEntry, objectType, 0x2);
CHECK_OFFSET(KHandleEntry, objectCompressedPointer, 0x4);
CHECK_SIZE(KHandleEntry, 0x8);
