#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KMemoryBlockList.h"
#include "KMutex.h"
#include "KProcess.h"
#include "structutils.h"

class KTransferMemory : public KAutoObject {
public:
    KTransferMemory(uint64_t baseAddress_, uint64_t size_, int32_t permissions_, ObjectPtr<KProcess> ownerProcess_);

    virtual ~KTransferMemory();

    virtual ObjectType getType() {
        return ObjectType::TRANSFER_MEM;
    }

    uint64_t getBaseAddress() { return baseAddress; };

private:
    KMemoryBlockList blocks;
    ObjectPtr<KProcess> ownerProcess;
    uint64_t baseAddress;
    KMutex mutex;
    int32_t permissions;
    bool hasInited;
    bool isMapped;
};
CHECK_SIZE(KTransferMemory, 0x48);
