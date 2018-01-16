#pragma once
#include <stdint.h>

#include "KMutex.h"
#include "KPageTable.h"
#include "KMemoryBlockManager.h"
#include "structutils.h"

struct KMemoryManager {
    uint64_t addrSpaceMinAddr;
    uint64_t addrSpaceMaxAddr;
    uint64_t heapBaseAddr;
    uint64_t heapEndAddr;
    uint64_t heapCurAddr;
    uint64_t mapRegionBaseAddr;
    uint64_t mapRegionEnd;
    uint64_t newMapRegionBaseAddr;
    uint64_t newMapRegionEnd;
    uint64_t tlsIoRegionBaseAddr;
    uint64_t tlsIoRegionEnd;
    uint64_t heapMaxAllocation;
    KMutex mutex;
    KPageTable pageTable;
    KMemoryBlockManager memoryBlockManager;
    uint32_t addressSpaceWidth;
    bool isKernel;
    bool isSystem;
    bool hasAslr;
    uint32_t poolPartition;
    uint32_t is_0x5A_If_Cfg12_Bit0_ElseZero;
    uint32_t is_0x59_If_Cfg12_Bit0_ElseZero;
    uint32_t is_0x58_If_Cfg12_Bit0_ElseZero;
    uint64_t translationTableBaseRegister0;
    uint64_t translationControlRegister;
    uint32_t asidTagValue;
    UNKNOWN(0x4);

    virtual void dummy() {} // force creation of vtable
};
CHECK_OFFSET(KMemoryManager, addrSpaceMinAddr, 0x8);
CHECK_OFFSET(KMemoryManager, addrSpaceMaxAddr, 0x10);
CHECK_OFFSET(KMemoryManager, heapBaseAddr, 0x18);
CHECK_OFFSET(KMemoryManager, heapEndAddr, 0x20);
CHECK_OFFSET(KMemoryManager, heapCurAddr, 0x28);
CHECK_OFFSET(KMemoryManager, mapRegionBaseAddr, 0x30);
CHECK_OFFSET(KMemoryManager, mapRegionEnd, 0x38);
CHECK_OFFSET(KMemoryManager, newMapRegionBaseAddr, 0x40);
CHECK_OFFSET(KMemoryManager, newMapRegionEnd, 0x48);
CHECK_OFFSET(KMemoryManager, tlsIoRegionBaseAddr, 0x50);
CHECK_OFFSET(KMemoryManager, tlsIoRegionEnd, 0x58);
CHECK_OFFSET(KMemoryManager, heapMaxAllocation, 0x60);
CHECK_OFFSET(KMemoryManager, mutex, 0x68);
CHECK_OFFSET(KMemoryManager, pageTable, 0x70);
CHECK_OFFSET(KMemoryManager, memoryBlockManager, 0x80);
CHECK_OFFSET(KMemoryManager, addressSpaceWidth, 0x98);
CHECK_OFFSET(KMemoryManager, isKernel, 0x9C);
CHECK_OFFSET(KMemoryManager, isSystem, 0x9D);
CHECK_OFFSET(KMemoryManager, hasAslr, 0x9E);
CHECK_OFFSET(KMemoryManager, poolPartition, 0xA0);
CHECK_OFFSET(KMemoryManager, is_0x5A_If_Cfg12_Bit0_ElseZero, 0xA4);
CHECK_OFFSET(KMemoryManager, is_0x59_If_Cfg12_Bit0_ElseZero, 0xA8);
CHECK_OFFSET(KMemoryManager, is_0x58_If_Cfg12_Bit0_ElseZero, 0xAC);
CHECK_OFFSET(KMemoryManager, translationTableBaseRegister0, 0xB0);
CHECK_OFFSET(KMemoryManager, translationControlRegister, 0xB8);
CHECK_OFFSET(KMemoryManager, asidTagValue, 0xC0);
CHECK_SIZE(KMemoryManager, 0xC8);
