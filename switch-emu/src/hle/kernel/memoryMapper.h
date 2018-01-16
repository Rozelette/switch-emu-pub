#pragma once
#include <stdint.h>
#include <vector>

#include "mem.h"

namespace kernel::memmap {

// TODO better names
enum class MemoryType : uint8_t {
    UNMAPPED = 0x0,
    IO = 0x1,
    NORMAL = 0x2,
    CODE_STATIC = 0x3,
    CODE = 0x4,
    HEAP = 0x5,
    SHARED_MEM_BLOCK = 0x6,
    WEIRD_MAPPED_MEM = 0x7,
    MODULE_CODE_STATIC = 0x8,
    MODULE_CODE_MUTABLE = 0x9,
    IPC_BUFFER_0 = 0xA,
    MAPPED_MEM = 0xB,
    THEAD_LOCAL_STORAGE = 0xC,
    ISOLATED_TRANSFER_MEM = 0xD,
    TRANSFER_MEM = 0xE,
    PROCESS_MEM = 0xF,
    RESERVED = 0x10,
    IPC_BUFFER_1 = 0x11,
    IPC_BUFFER_2 = 0x12,
    UNKNOWN = 0x13
};

// TODO verify bit layout
struct MemoryState {
    MemoryType type : 8;
    uint8_t permissionChangeAllowed : 1;
    uint8_t unk1 : 5;
    uint8_t processPermissionChangeAllowed : 1;
    uint8_t mapAllowed : 1;
    uint8_t unmapProcessCodeMemoryAllowed : 1;
    uint8_t transferMemoryAllowed : 1;
    uint8_t unk2 : 1;
    uint8_t mapDeviceAllowed : 1;
    uint8_t mapDeviceAlignedAllowed : 1;
    uint8_t ipcBufferAllowed : 1;
    uint8_t isCached : 1; // ?
    uint8_t mapProcessAllowed : 1;
    uint8_t attributeChangeAllowed : 1;
    uint8_t pad : 7;
};

struct MemoryMapping {
    uint64_t base;
    uint64_t size;
    MemoryState state;
    mem::Permission permissions;

    MemoryMapping(uint64_t base_, uint64_t size_, MemoryType type, mem::Permission permissions_) :
        base(base_), size(size_), permissions(permissions_) {
        state.type = type;
        state.permissionChangeAllowed = 0;
        state.unk1 = 0;
        state.processPermissionChangeAllowed = 0;
        state.mapAllowed = 0;
        state.unmapProcessCodeMemoryAllowed = 0;
        state.transferMemoryAllowed = 0;
        state.unk2 = 0;
        state.mapDeviceAllowed = 0;
        state.mapDeviceAlignedAllowed = 0;
        state.ipcBufferAllowed = 0;
        state.isCached = 0;
        state.mapProcessAllowed = 0;
        state.attributeChangeAllowed = 0;
        state.pad = 0;
    }

    bool operator<(const MemoryMapping& other) {
        return this->base < other.base;
    }
};

void addMapping(uint64_t base, uint64_t size, MemoryType type, mem::Permission permissions);

const MemoryMapping& findMapping(uint64_t addr);

void printMappings();

} // namespace kernel::memmap