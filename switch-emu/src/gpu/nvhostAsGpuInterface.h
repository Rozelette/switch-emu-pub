#pragma once
#include "deviceInterface.h"

namespace gpu {

class NvhostAsGpuInterface : public DeviceInterface {

    uint32_t BindChannel(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t AllocSpace(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t MapBufferEx(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t Initialize(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t GetVaRegions(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t InitializeEx(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);

public:
    NvhostAsGpuInterface() {
        registerFunctions({
            { 1, static_cast<InterfaceFunc>(&NvhostAsGpuInterface::BindChannel), "BindChannel" },
            { 2, static_cast<InterfaceFunc>(&NvhostAsGpuInterface::AllocSpace), "AllocSpace" },
            { 6, static_cast<InterfaceFunc>(&NvhostAsGpuInterface::MapBufferEx), "MapBufferEx" },
            { 7, static_cast<InterfaceFunc>(&NvhostAsGpuInterface::Initialize), "Initialize" },
            { 8, static_cast<InterfaceFunc>(&NvhostAsGpuInterface::GetVaRegions), "GetVaRegions" },
            { 9, static_cast<InterfaceFunc>(&NvhostAsGpuInterface::InitializeEx), "InitializeEx" },
        });
    }

    std::string getPath() {
        return "/dev/nvhost-as-gpu";
    }

private:
    // TODO own .h?
    struct BindChannelArgs {
        uint32_t fd;
    };

    struct AllocSpaceArgs {
        uint32_t pages;
        uint32_t pageSize;
        uint32_t flags;
        uint32_t pad = 0;
        union {
            uint64_t offset;
            uint64_t align;
        };
    };

    struct MapBufferExArgs {
        uint32_t flags;
        uint32_t kind;
        uint32_t nvmapHandle;
        uint32_t pageSize;
        uint64_t bufferOffset;
        uint64_t mappingSize;
        uint64_t offset;
    };

    struct InitializeArgs {
        uint32_t bigPageSize;
        int32_t asFd;
        uint32_t flags;
        uint32_t reserved = 0;
    };

    struct VaRegion {
        uint64_t offset;
        uint32_t pageSize;
        uint32_t reserved = 0;
        uint64_t pages;
    };

    struct GetVaRegionsArgs {
        uint64_t unusedPointer;
        uint32_t bufsize;
        uint32_t reserved = 0;
        VaRegion regions[2];
    };

    struct InitializeExArgs {
        uint32_t bigPageSize;
        int32_t asFd;
        uint32_t flags;
        uint32_t reserved = 0;
        uint64_t unk0;
        uint64_t unk1;
        uint64_t unk2;
    };
};

} // namespace gpu
