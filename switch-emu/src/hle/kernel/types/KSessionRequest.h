#pragma once
#include <stdint.h>

#include "KLinkedListNode.h"
#include "KBufferDescriptor.h"
#include "structutils.h"

struct KThread;
struct KWritableEvent;

struct KSessionRequest {
    UNKNOWN(0x08);
    KLinkedListNode<> requestList;
    KBufferDescriptor bufferTable[8];
    KBufferDescriptor* externalBufferTable;
    uint8_t numSendBuffers;
    uint8_t numRecvBuffers;
    uint8_t numExchBuffers;
    KThread* senderThread;
    uint64_t initiallyZero; // ???
    KWritableEvent* forAsyncEvent;
    uint64_t customCmdbufAddr;
    uint64_t customCmdbufSize;

    virtual void dummy() {} // force creation of vtable
};
CHECK_OFFSET(KSessionRequest, requestList, 0x10);
CHECK_OFFSET(KSessionRequest, bufferTable, 0x20);
CHECK_OFFSET(KSessionRequest, externalBufferTable, 0x120);
CHECK_OFFSET(KSessionRequest, numSendBuffers, 0x128);
CHECK_OFFSET(KSessionRequest, numRecvBuffers, 0x129);
CHECK_OFFSET(KSessionRequest, numExchBuffers, 0x12A);
CHECK_OFFSET(KSessionRequest, senderThread, 0x130);
CHECK_OFFSET(KSessionRequest, initiallyZero, 0x138);
CHECK_OFFSET(KSessionRequest, forAsyncEvent, 0x140);
CHECK_OFFSET(KSessionRequest, customCmdbufAddr, 0x148);
CHECK_OFFSET(KSessionRequest, customCmdbufSize, 0x150);
CHECK_SIZE(KSessionRequest, 0x158);

