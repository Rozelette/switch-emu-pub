#include "nvhostAsGpuInterface.h"
#include "hle\kernel\memoryMapper.h"
#include "log.h"
#include "nvmapInterface.h"

namespace gpu {

uint32_t NvhostAsGpuInterface::BindChannel(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    BindChannelArgs objectIn;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command BindChannel");
    logger::info("BindChannelArgs");
    logger::info("fd: %08X", objectIn.fd);

    return ERR_SUCCESS;
}

uint32_t NvhostAsGpuInterface::AllocSpace(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    // libtransistor calles this with pages = 0x10000, pageSize = 0x10000, and expects align = 0x400000000 (equal to offset in VA w/ pgsize = 0x10000)
    // Does this reserve chunks of VA based on the pageSize?
    AllocSpaceArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command AllocSpace");
    logger::info("AllocSpaceArgs");
    logger::info("pages: %08X", objectIn.pages);
    logger::info("pageSize: %08X", objectIn.pageSize);
    logger::info("flags: %08X", objectIn.flags);
    logger::info("pad: %08X", objectIn.pad);
    logger::info("offset: %016X", objectIn.offset);
    logger::info("align: %016X", objectIn.align);

    // TODO not use some made-up mapping
    // Does it actually make a mapping?
    kernel::memmap::addMapping(0xF0000000, 0x10000000, kernel::memmap::MemoryType::CODE_STATIC, mem::Permission::ALL);

    objectOut.pages = objectIn.pages;
    objectOut.pageSize = objectIn.pageSize;
    objectOut.flags = objectIn.flags;
    objectOut.pad = 0;
    objectOut.offset = 0x400000000;

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvhostAsGpuInterface::MapBufferEx(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    MapBufferExArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command MapBufferEx");
    logger::info("MapBufferExArgs");
    logger::info("flags: %08X", objectIn.flags);
    logger::info("kind: %08X", objectIn.kind);
    logger::info("nvmapHandle: %08X", objectIn.nvmapHandle);
    logger::info("pageSize: %08X", objectIn.pageSize);
    logger::info("bufferOffset: %016X", objectIn.bufferOffset);
    logger::info("mappingSize: %016X", objectIn.mappingSize);
    logger::info("offset: %016X", objectIn.offset);

    objectOut.pageSize = objectIn.pageSize;
    objectOut.offset = lookupNvmapObject(objectIn.nvmapHandle).addr; // TODO what is the actual offset?

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvhostAsGpuInterface::Initialize(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    InitializeArgs objectIn;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command Initialize");
    logger::info("InitializeArgs");
    logger::info("bigPageSize: %08X", objectIn.bigPageSize);
    logger::info("asFd: %08X", objectIn.asFd);
    logger::info("flags: %08X", objectIn.flags);
    logger::info("reserved: %08X", objectIn.reserved);

    return ERR_SUCCESS;
}

uint32_t NvhostAsGpuInterface::GetVaRegions(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    // Seems to change between calls. Values are from https://github.com/reswitched/libtransistor/blob/master/test/test_nv_ioc_templates.h
    GetVaRegionsArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command GetVaRegions");
    logger::info("GetVaRegionsArgs");
    logger::info("unusedPointer: %016X", objectIn.unusedPointer);
    logger::info("bufsize: %08X", objectIn.bufsize);
    logger::info("reserved: %08X", objectIn.reserved);

    static int timesCalled = 0;

    if (timesCalled == 0) {
        objectOut.regions[0].offset = 0;
        objectOut.regions[0].pages = 0;
        objectOut.regions[0].pageSize = 0;

        objectOut.regions[1].offset = 0;
        objectOut.regions[1].pages = 0;
        objectOut.regions[1].pageSize = 0;

        objectOut.bufsize = 2 * sizeof(VaRegion);
        objectOut.unusedPointer = 0;
    } else {
        if (timesCalled >= 2) {
            logger::warn("GetVaRegions called for third or more time");
        }

        objectOut.regions[0].offset = 0x04000000;
        objectOut.regions[0].pages = 0x3fbfff;
        objectOut.regions[0].pageSize = 0x1000;

        objectOut.regions[1].offset = 0x0400000000;
        objectOut.regions[1].pages = 0x1bffff;
        objectOut.regions[1].pageSize = 0x10000;

        objectOut.bufsize = 2 * sizeof(VaRegion);
        objectOut.unusedPointer = 0x6fff8f0;
    }

    timesCalled++;

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvhostAsGpuInterface::InitializeEx(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    InitializeExArgs objectIn;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command InitializeEx");
    logger::info("InitializeExArgs");
    logger::info("bigPageSize: %08X", objectIn.bigPageSize);
    logger::info("asFd: %08X", objectIn.asFd);
    logger::info("flags: %08X", objectIn.flags);
    logger::info("reserved: %08X", objectIn.reserved);
    logger::info("unk0: %016X", objectIn.unk0);
    logger::info("unk1: %016X", objectIn.unk1);
    logger::info("unk2: %016X", objectIn.unk2);

    return ERR_SUCCESS;
}

}