#pragma once
#include <stdint.h>
#include <atomic>
#include <unordered_set>
#include <intrin.h>

#include "cpu.h"

namespace mem {

const uint64_t VMEM_SIZE = (1llu << 38);
const uint64_t PAGE_BITS = 12;
const uint64_t PAGE_SIZE = (1 << PAGE_BITS);
const uint64_t PAGE_OFFSET_MASK = PAGE_SIZE - 1;
const uint64_t NUM_PAGES = VMEM_SIZE >> PAGE_BITS;

enum class Permission : uint8_t {
    NONE = 0,
    READ_ONLY = 1,
    WRITE_ONLY = 2,
    READ_WRITE = 3,
    EXECUTE_ONLY = 4,
    READ_EXECUTE = 5,
    WRITE_EXECUTE = 6,
    ALL = 7
};

constexpr uint64_t getPageNum(uint64_t addr) {
    return addr >> PAGE_BITS;
}

constexpr bool isPageAligned(uint64_t addr) {
    return (addr & PAGE_OFFSET_MASK) == 0;
}

constexpr uint64_t alignToPage(uint64_t addr) {
    return addr & ~PAGE_OFFSET_MASK;
}

extern std::unordered_set<uint64_t> gMemBreakpoints;

bool init(uint64_t memSize);
void fini();
void writeRegion(uint64_t addr, const uint8_t* data, size_t len);
void readRegion(uint64_t addr, uint8_t* data, size_t len);
void zeroMem(uint64_t addr, size_t len);

uint8_t* translate(uint64_t addr);
bool isValid(uint64_t addr);
Permission getPermission(uint64_t addr);

bool allocatePage(uint64_t pageNum);
void freePage(uint64_t pageNum);
void protectPage(uint64_t pageNum, Permission perm);

bool allocateRegion(uint64_t beginPageNum, uint64_t numPages);
void freeRegion(uint64_t beginPageNum, uint64_t numPages);
void protectRegion(uint64_t beginPageNum, uint64_t numPages, Permission perm);

template<typename T>
inline T read(uint64_t addr) {
    if (gMemBreakpoints.count(addr) > 0) {
        logger::warn("mem Breakpoint hit reading %016llX", addr);
        cpu::sendInterrupts(cpu::this_core::getCoreID(), cpu::DEBUG_BREAK_INTERRUPT);
    }
    T ret = *reinterpret_cast<T*>(translate(addr));
    //printf("Read %016llX at %016llX\n", static_cast<uint64_t>(ret), addr);
    return ret;
}

template<typename T>
inline T inspect(uint64_t addr) {
    T ret = *reinterpret_cast<T*>(translate(addr));
    return ret;
}

inline uint32_t fetch(uint64_t addr) {
    uint32_t ret = *reinterpret_cast<uint32_t*>(translate(addr));
    return ret;
}

template<typename T>
inline void write(uint64_t addr, T data) {
    if (gMemBreakpoints.count(addr) > 0) {
        logger::warn("mem Breakpoint hit, writing %016llX to %016llX", data, addr);
        cpu::sendInterrupts(cpu::this_core::getCoreID(), cpu::DEBUG_BREAK_INTERRUPT);
    }
    *reinterpret_cast<T*>(translate(addr)) = data;
    //printf("Writing %016llX to %016llX\n", static_cast<uint64_t>(data), addr);
}

// TODO 128b
template<typename T>
inline bool writeAtomic(uint64_t addr, T data, T expected) {
    auto atomicWriter = reinterpret_cast<std::atomic<T>*>(translate(addr));
    return atomicWriter->compare_exchange_strong(expected, data);
}

inline bool writeAtomic128(uint64_t addr, uint64_t dataLo, uint64_t dataHigh, uint64_t expectedLo, uint64_t expectedHigh) {
    int64_t* dst = reinterpret_cast<int64_t*>(translate(addr));
    uint64_t compare[2];
    compare[0] = expectedLo;
    compare[1] = expectedHigh;
    // TODO this requires 16B alignment, but AArch64 only requires 8B alignment, so this could cause issues
    // TODO use a generic function and not an intrinsic
    return _InterlockedCompareExchange128(dst, dataLo, dataHigh, reinterpret_cast<int64_t*>(compare));
}

} // namespace mem