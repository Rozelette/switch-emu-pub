#pragma once
#include "deviceInterface.h"

namespace gpu {

// TODO proper object
struct NvmapObject {
    uint32_t handle;
    uint32_t id;
    uint32_t size;
    uint32_t align;
    uint32_t heap;
    uint8_t kind;
    uint64_t addr;
};

NvmapObject& lookupNvmapObject(uint32_t handle);

class NvmapInterface : public DeviceInterface {

    uint32_t IocCreate(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t IocFromId(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t IocAlloc(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t IocParam(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    uint32_t IocGetId(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);

public:
    NvmapInterface() {
        registerFunctions({
        { 1, static_cast<InterfaceFunc>(&NvmapInterface::IocCreate), "IocCreate" },
        { 3, static_cast<InterfaceFunc>(&NvmapInterface::IocFromId), "IocFromId" },
        { 4, static_cast<InterfaceFunc>(&NvmapInterface::IocAlloc), "IocAlloc" },
        { 9, static_cast<InterfaceFunc>(&NvmapInterface::IocParam), "IocParam" },
        { 14, static_cast<InterfaceFunc>(&NvmapInterface::IocGetId), "IocGetId" },
        });
    }

    std::string getPath() {
        return "/dev/nvmap";
    }

private:
    // TODO own .h?
    struct IocCreateArgs {
        uint32_t size;
        uint32_t handle;
    };

    struct IocFromIdArgs {
        uint32_t handle;
        uint32_t id;
    };

    struct IocAllocArgs {
        uint32_t handle;
        uint32_t heapMask;
        uint32_t flags;
        uint32_t align;
        uint8_t kind;
        uint8_t pad[7];
        uint64_t addr;
    };

    enum class IocParamType : uint32_t {
        SIZE = 1,
        ALIGNMENT = 2,
        BASE = 3,
        HEAP = 4,
        KIND = 5,
        COMPR = 6,
    };

    struct IocParamArgs {
        uint32_t handle;
        IocParamType param;
        uint32_t result;
    };

    struct IocGetIdArgs {
        uint32_t handle;
        uint32_t id;
    };
};

} // namespace gpu
