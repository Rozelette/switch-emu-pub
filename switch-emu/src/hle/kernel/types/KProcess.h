#pragma once
#include <stdint.h>

#include "KSynchronizationObject.h"
#include "KProcessScheduler.h"
#include "KMemoryManager.h"
#include "KResourceLimit.h"
#include "KMutex.h"
#include "KProcessCapabilities.h"
#include "KProcessHandleTable.h"
#include "KLinkedList.h"
#include "KTlsPageManager.h"
#include "structutils.h"

class KDebug;
class KThread;
class KSharedMemoryRef;

struct CreateProcessInfo {
    // TODO
};

class KProcess : public KSynchronizationObject {

public:
    KProcess(const CreateProcessInfo& info);

private:
    KProcessScheduler scheduler;
    KMemoryManager memoryManager;
    uint64_t totalMemUsage;
    KLinkedList<KTlsPageManager*> tlsPagesList;
    int32_t defaultCpuCore;
    KDebug* debug;
    KResourceLimit* resourceLimit;
    uint32_t state;
    PADDING(0x4);
    KMutex processMutex;
    KMutex threadingMutex;
    KLinkedListNode<KThread> theadArbiterList;
    KLinkedListNode<> unkList;
    uint64_t randomEntropy[4];
    uint8_t hasStateChanged;
    uint8_t hasInitialized;
    uint8_t isSystem; // from CreateProcessInfo 0x24 Bit6
    uint8_t processName[12 + 1]; // from CreateProcessInfo 0[12]
    uint16_t numberOfCreatedThreads;
    uint16_t threadingRelatedInitiallyZero; // ???
    uint32_t processFlags;
    KProcessCapabilities capabilities;
    uint64_t titleId;
    uint64_t processId;
    uint64_t createdTickStamp;
    uint64_t entryPoint; // from CreateProcessInfo 0x18
    uint64_t codeMemUsage;
    uint64_t dynamicMemUsage;
    uint64_t maxTotalMemUsage;
    uint64_t from_CreateProcessInfo_0xC; // ???
    uint64_t unused0[5]; // ???
    KProcessHandleTable handleTable;
    void* usermodeExceptionTlsArea;
    KLinkedListNode<KThread> exceptionThreadList;
    KThread* exceptionThread;
    KLinkedListNode<KThread> threadList;
    KLinkedListNode<KSharedMemoryRef> mappedSharedMemoriesList;
    bool hasStarted;
    bool hasException;
    uint32_t exceptionEnum0;
    uint64_t exceptionEnum1;
    uint64_t exceptionThreadUnkInfo0;
    uint64_t breakArg0;
    uint64_t breakArg1;
    uint64_t breakArg2;
    uint64_t exceptionThreadUnkInfo1;
    uint32_t unusedZero0; // ???
    uint64_t unusedZeroes[8]; // ???
    uint64_t unusedZero1; // ???
    uint64_t unusedZero2; // ???
    uint64_t unused1[6]; // ???
};
CHECK_SIZE(KProcess, 0x2480); // In 2.0.0, 0x2580 in 3.0.0
