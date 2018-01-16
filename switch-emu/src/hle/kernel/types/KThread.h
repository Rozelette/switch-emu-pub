#pragma once
#include <stdint.h>

#include "KSynchronizationObject.h"
#include "KLinkedListNode.h"
#include "structutils.h"

class KProcess;

struct KSchedulerObject0 {
    UNKNOWN(0x20);
};
CHECK_SIZE(KSchedulerObject0, 0x20);

struct KSchedulerObject1 {
    UNKNOWN(0x10);
};
CHECK_SIZE(KSchedulerObject1, 0x10);

struct KSchedulerObject2 {
    UNKNOWN(0x18);
};
CHECK_SIZE(KSchedulerObject2, 0x18);

// Guess: 32 VRegs (0x200) + 31 GPRegs (0x78) + 8 bytes of something (compressed PSTATE + FPCR + FPSR?)
// PC, SP, and TLS are stored in KThread, I suppose
struct KThreadContext {
    UNKNOWN(0x280);
};
CHECK_SIZE(KThreadContext, 0x280);


class KThread : public KSynchronizationObject, KSchedulerObject0, KSchedulerObject1, KSchedulerObject2 {
public:
    KThreadContext context;
    uint64_t coreMask;
    uint64_t coreMaskOverride;
    uint64_t threadId;
    uint64_t totalTimeRunning;
    KSynchronizationObject* signalledObjectPtr;
    UNKNOWN(0x8);
    UNKNOWN(0x8);
    uint64_t entryPoint;
    uint64_t arbitrationUserPtr;
    KProcess* process;
    uint64_t kernelStackPointer;
    UNKNOWN(0x8);
    uint64_t tlsKernelPointer;
    uint64_t tlsPhysicalPointer;
    KSynchronizationObject* syncObjects[0x40];
    UNKNOWN(0x8);
    UNKNOWN(0x8);
    KSynchronizationObject* signalledObjectPtr2; // ?
    KLinkedListNode<> unk0;
    KLinkedListNode<> unk1;
    KLinkedListNode<> threadList;
    KLinkedListNode<> threadsOnSameCore;
    UNKNOWN(0x8);
    UNKNOWN(0x8);
    UNKNOWN(0x8);
    UNKNOWN(0x8);
    UNKNOWN(0x8);
    uint32_t arbitrationReqThreadHandle;
    uint32_t defaultSchedulingFlags;
    uint32_t waitSyncRetVal;
    UNKNOWN(0x4);
    uint32_t actualPriority;
    UNKNOWN(0x4);
    UNKNOWN(0x4);
    UNKNOWN(0x4);
    uint32_t wantedPriority;
    uint32_t cpuCore;
    uint8_t state;
    uint8_t shallBeTerminated;
    UNKNOWN(0x1);
    UNKNOWN(0x1);
    UNKNOWN(0x1);
    UNKNOWN(0x1);
    uint8_t hasExited;
    uint8_t hasCpuCoreOveride;
    UNKNOWN(0x10);
};
CHECK_SIZE(KThread, 0x620);
