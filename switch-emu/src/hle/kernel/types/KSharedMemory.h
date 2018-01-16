#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KMemoryBlockList.h"
#include "structutils.h"

class KProcess;

struct KSharedMemory : public KAutoObject {
    KMemoryBlockList blocks;
    KProcess* ownerProcess;
    uint64_t ownerProcessPid;
    int32_t localPerm;
    int32_t remotePerm;
    bool hasInited;
};
CHECK_OFFSET(KSharedMemory, blocks, 0x10);
CHECK_OFFSET(KSharedMemory, ownerProcess, 0x28);
CHECK_OFFSET(KSharedMemory, ownerProcessPid, 0x30);
CHECK_OFFSET(KSharedMemory, localPerm, 0x38);
CHECK_OFFSET(KSharedMemory, remotePerm, 0x3C);
CHECK_OFFSET(KSharedMemory, hasInited, 0x40);
CHECK_SIZE(KSharedMemory, 0x48);
