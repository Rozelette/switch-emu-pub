#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KMemoryBlockList.h"
#include "KMutex.h"
#include "structutils.h"

class KProcess;

struct KJitMemory : public KAutoObject {
    PADDING(0x10); // KAutoObject is 0x20 in 4.0.0?
    KMemoryBlockList blocks;
    KProcess* srcProcess;
    uint64_t srcAddr;
    KMutex mutex;
    bool hasInited;
    bool hasMappedSlave;
    bool hasMappedMaster;
    PADDING(0x5);
};
CHECK_OFFSET(KJitMemory, blocks, 0x20);
CHECK_OFFSET(KJitMemory, srcProcess, 0x38);
CHECK_OFFSET(KJitMemory, srcAddr, 0x40);
CHECK_OFFSET(KJitMemory, mutex, 0x48);
CHECK_OFFSET(KJitMemory, hasInited, 0x50);
CHECK_OFFSET(KJitMemory, hasMappedSlave, 0x51);
CHECK_OFFSET(KJitMemory, hasMappedMaster, 0x52);
CHECK_SIZE(KJitMemory, 0x58);
