#include <list>

#include "memoryMapper.h"
#include "log.h"
#include "unreachable.h"

// TODO:
// what are the mappings for unmapped regions? (i.e. past loaded code)
// error checking
namespace kernel::memmap {

std::list<MemoryMapping> gMappings;

std::string getPermissionString(mem::Permission permissions) {
    switch (permissions) {
    case mem::Permission::NONE:
        return "---";
    case mem::Permission::READ_ONLY:
        return "r--";
    case mem::Permission::WRITE_ONLY:
        return "-w-";
    case mem::Permission::READ_WRITE:
        return "rw-";
    case mem::Permission::EXECUTE_ONLY:
        return "--x";
    case mem::Permission::READ_EXECUTE:
        return "r-x";
    case mem::Permission::WRITE_EXECUTE:
        return "-wx";
    case mem::Permission::ALL:
        return "rwx";
    default:
        UNREACHABLE();
        return "---";
    }
}

std::string getMemTypeString(MemoryType type) {
    switch (type) {
    case kernel::memmap::MemoryType::UNMAPPED:
        return "UNMAPPED";
    case kernel::memmap::MemoryType::IO:
        return "IO";
    case kernel::memmap::MemoryType::NORMAL:
        return "NORMAL";
    case kernel::memmap::MemoryType::CODE_STATIC:
        return "CODE_STATIC";
    case kernel::memmap::MemoryType::CODE:
        return "CODE";
    case kernel::memmap::MemoryType::HEAP:
        return "HEAP";
    case kernel::memmap::MemoryType::SHARED_MEM_BLOCK:
        return "SHARED_MEM_BLOCK";
    case kernel::memmap::MemoryType::WEIRD_MAPPED_MEM:
        return "WEIRD_MAPPED_MEM";
    case kernel::memmap::MemoryType::MODULE_CODE_STATIC:
        return "MODULE_CODE_STATIC";
    case kernel::memmap::MemoryType::MODULE_CODE_MUTABLE:
        return "MODULE_CODE_MUTABLE";
    case kernel::memmap::MemoryType::IPC_BUFFER_0:
        return "IPC_BUFFER_0";
    case kernel::memmap::MemoryType::MAPPED_MEM:
        return "MAPPED_MEM";
    case kernel::memmap::MemoryType::THEAD_LOCAL_STORAGE:
        return "THEAD_LOCAL_STORAGE";
    case kernel::memmap::MemoryType::ISOLATED_TRANSFER_MEM:
        return "ISOLATED_TRANSFER_MEM";
    case kernel::memmap::MemoryType::TRANSFER_MEM:
        return "TRANSFER_MEM";
    case kernel::memmap::MemoryType::PROCESS_MEM:
        return "PROCESS_MEM";
    case kernel::memmap::MemoryType::RESERVED:
        return "RESERVED";
    case kernel::memmap::MemoryType::IPC_BUFFER_1:
        return "IPC_BUFFER_1";
    case kernel::memmap::MemoryType::IPC_BUFFER_2:
        return "IPC_BUFFER_2";
    case kernel::memmap::MemoryType::UNKNOWN:
        return "UNKNOWN";
    default:
        UNREACHABLE();
        return "X";
    }
}

// TODO must these be page-aligned?
void addMapping(uint64_t base, uint64_t size, MemoryType type, mem::Permission permissions) {
    logger::info("Creating memory region at %016llX with size %016llX of type %02X with permissions %s",
        base, size, static_cast<uint8_t>(type), getPermissionString(permissions).c_str());

    uint64_t newBase = base;
    uint64_t newEnd = base + size - 1;
    for (auto iter = gMappings.begin(); iter != gMappings.end(); iter++) {
        MemoryMapping& mapping = *iter;
        uint64_t mapBase = mapping.base;
        uint64_t mapEnd = mapping.base + mapping.size - 1;
        if (mapEnd < newBase) {
            // map: |------|
            // new:         |-----|
            continue;
        } else if ((mapEnd > newBase) && (mapEnd <= newEnd) && (mapBase < newBase)) {
            // map: |------|
            // new:      |-----|
            mapping.size = newBase - mapBase;
        } else if ((mapEnd >= newEnd) && (mapBase <= newBase)) {
            if ((mapBase == newBase) && (mapEnd == newEnd)) {
                // map: |-----|
                // new: |-----|
                iter = gMappings.erase(iter);
            } else if (mapBase == newBase) {
                // map: |-------------|
                // new: |-----|
                mapping.base = newEnd + 1;
                mapping.size = mapEnd - mapping.base + 1;
            } else if (mapEnd == newEnd) {
                // map: |-------------|
                // new:         |-----|
                mapping.size = newBase - mapBase;
            } else {
                // map: |-------------|
                // new:      |-----|
                mapping.size = newBase - mapBase;
                // directly insert a new mapping instead of calling this function as we know the space has no mapping
                gMappings.push_front(MemoryMapping(newEnd + 1, mapEnd - newEnd, mapping.state.type, mapping.permissions));
            }
        } else if ((mapEnd <= newEnd) && (mapBase >= newBase)) {
            // map:     |------|
            // new: |------------|
            iter = gMappings.erase(iter);
        } else if ((mapBase >= newBase) && (mapBase < newEnd) && (mapEnd > newEnd)) {
            // map:     |------|
            // new: |------|
            mapping.base = newEnd + 1;
            mapping.size = mapEnd - mapping.base + 1;
        } else if (mapBase > newEnd) {
            // map:        |------|
            // new: |-----|
            continue;
        }
    }

    gMappings.push_front(MemoryMapping(base, size, type, permissions));

    // TODO do we really need to sort? I just want the printing to look better
    // TODO just use a struct that keeps things sorted?
    gMappings.sort();

    // TODO better way of tracking for mappings in actual memory
    if (type != MemoryType::UNMAPPED) {
        // TODO errors
        mem::allocateRegion(base / mem::PAGE_SIZE, size / mem::PAGE_SIZE);
        mem::protectRegion(base / mem::PAGE_SIZE, size / mem::PAGE_SIZE, permissions);
    }
}

// TODO better return value for invalid mappings?
const MemoryMapping& findMapping(uint64_t addr) {
    for (auto& mapping : gMappings) {
        if ((addr >= mapping.base) && (addr < (mapping.base + mapping.size))) {
            return mapping;
        }
    }

    return MemoryMapping(0, 0, MemoryType::RESERVED, mem::Permission::NONE);
}

void printMappings() {
    logger::info("Mappings:");
    for (auto& mapping : gMappings) {
        logger::info("0x%016llX-0x%016llX %s %s",
            mapping.base,
            mapping.base + mapping.size - 1,
            getPermissionString(mapping.permissions).c_str(),
            getMemTypeString(mapping.state.type).c_str());
    }
}

} // namespace kernel::memmap