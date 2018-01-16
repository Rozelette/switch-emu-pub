#pragma once
#include <stdint.h>

#include "KAutoObject.h"
#include "KLinkedListNode.h"
#include "structutils.h"

#include "hle\kernel\handleManager.h"

class KSynchronizationObject : public KAutoObject {
public:
    KSynchronizationObject() : threadSyncNum(), threadSyncList() {}

    virtual void sync(handle_t handle) {}; // TODO
    virtual void removeHandle(handle_t handle) {}; // TODO

private:
    uint64_t threadSyncNum;
    KLinkedListNode<> threadSyncList;
};
CHECK_SIZE(KSynchronizationObject, 0x28);

