#pragma once
#include "deviceInterface.h"

namespace gpu {

// TODO channel object parent
class NvhostGpuInterface : public DeviceInterface {

    uint32_t SetNvmapFd(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t AllocObjCtx(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t ZcullBind(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t SetErrorNotifier(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t SetPriority(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t SetUserData(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t AllocGpfifoEx2(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);

public:
    NvhostGpuInterface() {
        registerFunctions({
            // TODO are channel commands more complicated than just an id?
            { 1, static_cast<InterfaceFunc>(&NvhostGpuInterface::SetNvmapFd), "SetNvmapFd" },
            { 9, static_cast<InterfaceFunc>(&NvhostGpuInterface::AllocObjCtx), "AllocObjCtx" },
            { 11, static_cast<InterfaceFunc>(&NvhostGpuInterface::ZcullBind), "ZcullBind" },
            { 12, static_cast<InterfaceFunc>(&NvhostGpuInterface::SetErrorNotifier), "SetErrorNotifier" },
            { 13, static_cast<InterfaceFunc>(&NvhostGpuInterface::SetPriority), "SetPriority" },
            { 20, static_cast<InterfaceFunc>(&NvhostGpuInterface::SetUserData), "SetUserData" },
            { 26, static_cast<InterfaceFunc>(&NvhostGpuInterface::AllocGpfifoEx2), "AllocGpfifoEx2" },
         });
    }

    std::string getPath() {
        return "/dev/nvhost-gpu";
    }

private:
    // TODO own .h?
    struct SetNvmapFdArgs {
        uint32_t nvmapFd;
    };

    struct AllocObjCtxArgs {
        uint32_t classNum;
        uint32_t flags;
        uint32_t objId; // ignored
    };

    struct ZcullBindArgs {
        uint64_t gpuVa;
        uint32_t mode; // 0 - global, 1 - no_ctxsw, 2 - seperate_buffer, 3 - part_of_regular_buf
        uint32_t padding;
    };

    struct SetErrorNotifierArgs {
        uint64_t offset;
        uint64_t size;
        uint32_t mem; // nvmap handle
        uint32_t pad;
    };

    struct SetPriorityArgs {
        uint32_t priority; // 0x32 - low, 0x64 - med, 0x96 - high
    };

    struct SetUserDataArgs {
        // TODO
    };

    // TODO pull out
    struct Fence {
        uint32_t id;
        uint32_t value;
    };

    struct AllocGpfifoEx2Args {
        uint32_t numEntries;
        uint32_t flags;
        uint32_t unk0;
        Fence fence;
        uint32_t unk1;
        uint32_t unk2;
        uint32_t unk3;
    };
};

} // namespace gpu
