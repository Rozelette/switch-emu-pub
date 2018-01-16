#include "KTransferMemory.h"

#include "hle\kernel\memoryMapper.h"

KTransferMemory::KTransferMemory(uint64_t baseAddress_, uint64_t size_, int32_t permissions_, ObjectPtr<KProcess> ownerProcess_) :
baseAddress(baseAddress_), permissions(permissions_), ownerProcess(ownerProcess_) {
    // TODO what is the actual mapping made?
    //kernel::memmap::addMapping(addr, size, kernel::memmap::MemoryType::TRANSFER_MEM, static_cast<mem::Permission>(permissions_));
    kernel::memmap::addMapping(baseAddress_, size_, kernel::memmap::MemoryType::TRANSFER_MEM, mem::Permission::ALL);
    kernel::memmap::printMappings();
}

KTransferMemory::~KTransferMemory() {
    // TODO unmap memory
}