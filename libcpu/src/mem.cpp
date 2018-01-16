#include <map>
#include <array>
#include <mutex>

#include "platform_mem.h"

#include "mem.h"
#include "bitutils.h"
#include "log.h"
#include "emu_assert.h"
#include "cpu.h"


namespace cpu {

extern SegFaultHandler gSegFaultHandler;

}

namespace mem {

struct PageTableEntry {
    uint8_t* pointer;
    Permission perm;
};

struct PageTable {
    std::array<PageTableEntry, NUM_PAGES> entires;
};

uint8_t* gFreePageList = nullptr;

std::mutex gPageAllocatorLock;

PageTable gMainPageTable;

uint8_t* gMemBase = nullptr;

std::unordered_set<uint64_t> gMemBreakpoints;

bool init(uint64_t memSize) {
    if (memSize == 0) {
        logger::error("memSize given to mem::init is 0");
        return false;
    }
    if (!isPageAligned(memSize)) {
        logger::error("memSize given to mem::init is not page aligned");
        return false;
    }

    auto page = platform::allocate(memSize, platform::PageFlags::READ_WRITE);
    if (!page) {
        logger::error("Could not allocated base memory");
        return false;
    } else {
        logger::info("Reserving base memory at %016llX", (uint64_t)page);
        gMemBase = reinterpret_cast<uint8_t*>(page);
    }

    uint64_t numPages = memSize / PAGE_SIZE;

    gFreePageList = gMemBase;
    for (auto i = 0u; i < (numPages - 1); i++) {
        *reinterpret_cast<uint8_t**>(gMemBase + i * PAGE_SIZE) = gMemBase + (i + 1) * PAGE_SIZE;
    }
    *reinterpret_cast<uint8_t**>(gMemBase + (numPages - 1) * PAGE_SIZE) = nullptr;

    //gMemBreakpoints.insert(0x287852d8);
    //gMemBreakpoints.insert(0x28784f78+0x1b8);

    //gMemBreakpoints.insert(0xaa00a7260);

    //gMemBreakpoints.insert(0x10BF50C0);

    //gMemBreakpoints.insert(0x10bf49e8);

    return true;
}

void fini() {
    if (gMemBase) {
        platform::free(gMemBase);
    }
}

bool allocatePage(uint64_t pageNum) {
    if (pageNum >= NUM_PAGES) {
        logger::error("Invalid page in allocatePage: 0x%016llX", pageNum);
        return false;
    }
    gPageAllocatorLock.lock();
    if (gMainPageTable.entires[pageNum].pointer != nullptr) {
        // page was already allocated, or was allocated by another thread
        gPageAllocatorLock.unlock();
        return true;
    }

    if (gFreePageList == nullptr) {
        logger::error("No free mem for allocatePage");
        return false;
    }

    uint8_t* page = gFreePageList;
    gFreePageList = *reinterpret_cast<uint8_t**>(gFreePageList);

    memset(page, 0, PAGE_SIZE);

    //logger::info("Allocating page %d for physical addr %016llX", baseMemIndex, pageNum << PAGE_BITS);
    gMainPageTable.entires[pageNum].pointer = page;
    gPageAllocatorLock.unlock();
    return true;
}

void freePage(uint64_t pageNum) {
    if (pageNum >= NUM_PAGES) {
        logger::error("Invalid page in freePage: 0x%016llX", pageNum);
        return;
    }
    gPageAllocatorLock.lock();

    uint8_t* freed = gMainPageTable.entires[pageNum].pointer;
    if (freed == nullptr) {
        return;
    }
    gMainPageTable.entires[pageNum].pointer = nullptr;
    gMainPageTable.entires[pageNum].perm = Permission::NONE;

    *reinterpret_cast<uint8_t**>(freed) = gFreePageList;
    gFreePageList = freed;

    gPageAllocatorLock.unlock();
}

void protectPage(uint64_t pageNum, Permission perm) {
    if (pageNum >= NUM_PAGES) {
        logger::error("Invalid page in protectPage: 0x%016llX", pageNum);
        return;
    }
    if (gMainPageTable.entires[pageNum].pointer == nullptr) {
        logger::error("Unmapped page in protectPage: 0x%016llX", pageNum);
        return;
    }
    gMainPageTable.entires[pageNum].perm = perm;
}

bool allocateRegion(uint64_t beginPageNum, uint64_t numPages) {
    for (auto i = 0u; i < numPages; i++) {
        if (!allocatePage(beginPageNum + i)) {
            return false;
        }
    }
    return true;
}

void freeRegion(uint64_t beginPageNum, uint64_t numPages) {
    for (auto i = 0u; i < numPages; i++) {
        freePage(beginPageNum + i);
    }
}

void protectRegion(uint64_t beginPageNum, uint64_t numPages, Permission perm) {
    for (auto i = 0u; i < numPages; i++) {
        protectPage(beginPageNum + i, perm);
    }
}

void writeRegion(uint64_t addr, const uint8_t* data, size_t len) {
    for (auto i = 0u; i < len;) {
        if ((((addr + i) & PAGE_OFFSET_MASK) == 0) &&
            ((i + PAGE_SIZE) < len)) {
            void* tanslatedAddr = reinterpret_cast<void*>(translate(addr + i));
            memcpy(tanslatedAddr, &data[i], PAGE_SIZE);
            i += PAGE_SIZE;
        } else {
            // TODO optimize
            write(addr + i, data[i]);
            i++;
        }
    }
}

void readRegion(uint64_t addr, uint8_t* data, size_t len) {
    for (auto i = 0u; i < len;) {
        if ((((addr + i) & PAGE_OFFSET_MASK) == 0) &&
            ((i + PAGE_SIZE) < len)) {
            void* tanslatedAddr = reinterpret_cast<void*>(translate(addr + i));
            memcpy(&data[i], tanslatedAddr, PAGE_SIZE);
            i += PAGE_SIZE;
        } else {
            // TODO optimize
            data[i] = read<uint8_t>(addr + i);
            i++;
        }
    }
}

void zeroMem(uint64_t addr, size_t len) {
    logger::info("Setting memory to zero, size %016llX at %016llX", len, addr);
    for (auto i = 0u; i < len;) {
        if ((((addr + i) & PAGE_OFFSET_MASK) == 0) &&
            ((i + PAGE_SIZE) < len)) {
            void* tanslatedAddr = reinterpret_cast<void*>(translate(addr + i));
            memset(tanslatedAddr, 0, PAGE_SIZE);
            i += PAGE_SIZE;
        } else {
            write(addr + i, 0);
            i++;
        }
    }
}

bool isValid(uint64_t addr) {
    uint64_t pageNum = getPageNum(addr);
    if (pageNum >= NUM_PAGES) {
        return false;
    }
    if (addr < PAGE_SIZE) {
        return false;
    }
    return gMainPageTable.entires[pageNum].pointer != nullptr;
}

Permission getPermission(uint64_t addr) {
    uint64_t pageNum = getPageNum(addr);
    if (pageNum >= NUM_PAGES) {
        return Permission::NONE;
    }
    return gMainPageTable.entires[getPageNum(addr)].perm;
}

uint8_t* translate(uint64_t addr) {
    uint64_t pageNum = getPageNum(addr);
    uint64_t pageOffset = addr & PAGE_OFFSET_MASK;
    if ((pageNum >= NUM_PAGES) || (gMainPageTable.entires[pageNum].pointer == nullptr)) {
        cpu::gSegFaultHandler(addr);
    }
    // TODO check perm based on access type
    //if (gMainPageTable.entires[pageNum].perm == Permission::NONE) {
    //    // TODO what fault handler is used? Page fault?
    //    logger::error("Permission error at 0x%016llX", addr);
    //    emu_abort("Permission error");
    //}

    return gMainPageTable.entires[pageNum].pointer + pageOffset;
}

} // namespace mem