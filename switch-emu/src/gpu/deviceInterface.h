#pragma once
#include <string>
#include <stdint.h>
#include <vector>

#include "hle\kernel\types\KAutoObject.h"

namespace gpu {

// TODO should this really be a KAutoObject?
class DeviceInterface : public KAutoObject {
public:
    using InterfaceFunc = uint32_t(DeviceInterface::*)(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);

    const static uint32_t ERR_SUCCESS = 0x0;
    const static uint32_t ERR_NOT_IMPLEMENTED = 0x1;
    const static uint32_t ERR_NOT_SUPPORTED = 0x2;
    const static uint32_t ERR_NOT_INITIALIZED = 0x3;
    const static uint32_t ERR_BAD_PARAMETER = 0x4;
    const static uint32_t ERR_TIMEOUT = 0x5;
    const static uint32_t ERR_INSUFFICIENT_MEMORY = 0x6;
    const static uint32_t ERR_READ_ONLY_ATTRIBUTE = 0x7;
    const static uint32_t ERR_INVALID_STATE = 0x8;
    const static uint32_t ERR_INVALID_ADDRESS = 0x9;
    const static uint32_t ERR_INVALID_SIZE = 0xA;
    const static uint32_t ERR_BAD_VALUE = 0xB;
    const static uint32_t ERR_ALREADY_ALLOCATED = 0xD;
    const static uint32_t ERR_BUSY = 0xE;
    const static uint32_t ERR_RESOURCE_ERROR = 0xF;
    const static uint32_t ERR_COUNT_MISMATCH = 0x10;
    const static uint32_t ERR_SHARED_MEMORY_TOO_SMALL = 0x1000;
    const static uint32_t ERR_FILE_OPERATION_FAILED = 0x30003;
    const static uint32_t ERR_IOCTL_FAILED = 0x3000f;

    // TODO expected In/Out? Size? Magic?
    struct FunctionEntry {
        uint32_t id;
        InterfaceFunc func;
        const char* name;
    };

    virtual uint32_t handleIoctl(uint32_t cmd, const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
    virtual std::string getPath() {
        return "UNKNOWN";
    }

    virtual ObjectType getType() {
        return ObjectType::GPU_DEVICE;
    }

protected:
    // TODO can this be avoided?
    void registerFunctions(const std::vector<FunctionEntry>& funcs);

    // TODO better place for these?
    template<class T>
    void writeObjectToBuffer(T& obj, std::vector<uint8_t>& buffer) {
        for (uint64_t i = 0; i < sizeof(T); i++) {
            buffer.push_back(reinterpret_cast<uint8_t*>(&obj)[i]);
        }
    }

    template<class T>
    void readObjectFromBuffer(T& obj, const std::vector<uint8_t>& buffer) {
        for (uint64_t i = 0; i < sizeof(T); i++) {
            reinterpret_cast<uint8_t*>(&obj)[i] = buffer[i];
        }
    }

private:
    std::vector<FunctionEntry> mFunctions;

};

} // namespace gpu