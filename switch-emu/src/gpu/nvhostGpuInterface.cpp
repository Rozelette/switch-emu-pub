#include "nvhostGpuInterface.h"
#include "log.h"

namespace gpu {

uint32_t NvhostGpuInterface::SetNvmapFd(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    SetNvmapFdArgs objectIn;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command SetNvmapFd");
    logger::info("SetNvmapFdArgs");
    logger::info("nvmapFd: 0x%08X", objectIn.nvmapFd);

    return ERR_SUCCESS;
}

uint32_t NvhostGpuInterface::AllocObjCtx(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    AllocObjCtxArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command AllocObjCtx");
    logger::info("AllocObjCtxArgs");
    logger::info("classNum: 0x%08X", objectIn.classNum);
    logger::info("flags: 0x%08X", objectIn.flags);
    logger::info("objId: 0x%08X", objectIn.objId);

    objectOut.objId = 0; // not used

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvhostGpuInterface::ZcullBind(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    ZcullBindArgs objectIn;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command ZcullBind");
    logger::info("ZcullBindArgs");
    logger::info("gpuVa: 0x%016llX", objectIn.gpuVa);
    logger::info("mode: 0x%08X", objectIn.mode);
    logger::info("padding: 0x%08X", objectIn.padding);

    return ERR_SUCCESS;
}

uint32_t NvhostGpuInterface::SetErrorNotifier(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    SetErrorNotifierArgs objectIn;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command SetErrorNotifier");
    logger::info("SetErrorNotifierArgs");
    logger::info("offset: 0x%016llX", objectIn.offset);
    logger::info("size: 0x%016llX", objectIn.size);
    logger::info("mem: 0x%08X", objectIn.mem);
    logger::info("pad: 0x%08X", objectIn.pad);

    return ERR_SUCCESS;
}

uint32_t NvhostGpuInterface::SetPriority(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    SetPriorityArgs objectIn;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command SetPriority");
    logger::info("SetPriorityArgs");
    logger::info("priority: 0x%08X", objectIn.priority);

    return ERR_SUCCESS;
}

uint32_t NvhostGpuInterface::SetUserData(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    SetUserDataArgs objectIn;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command SetUserData");
    logger::info("SetUserDataArgs");
    logger::info("TODO");

    return ERR_SUCCESS;
}

uint32_t NvhostGpuInterface::AllocGpfifoEx2(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    AllocGpfifoEx2Args objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command AllocGpfifoEx2");
    logger::info("AllocGpfifoEx2Args");
    logger::info("numEntries: 0x%08X", objectIn.numEntries);
    logger::info("flags: 0x%08X", objectIn.flags);
    logger::info("unk0: 0x%08X", objectIn.unk0);
    logger::info("fence.id: 0x%08X", objectIn.fence.id);
    logger::info("fence.value: 0x%08X", objectIn.fence.value);
    logger::info("unk1: 0x%08X", objectIn.unk1);
    logger::info("unk2: 0x%08X", objectIn.unk2);
    logger::info("unk3: 0x%08X", objectIn.unk3);

    static uint32_t fenceId = 1; // TODO remove
    objectOut.fence.id = fenceId++;
    objectOut.fence.value = 0;

    logger::info("Assigning fence id: 0x%08X", objectOut.fence.id);

    return ERR_SUCCESS;
}

} // namespace gpu