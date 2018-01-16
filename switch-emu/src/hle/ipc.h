#pragma once
#include <stdint.h>
#include <vector>

#include "kernel\handleManager.h"

namespace ipc {

struct PointerDescriptor {
    uint64_t counter;
    uint64_t address;
    uint64_t size;
};

struct RegularDescriptor {
    uint64_t flags;
    uint64_t address;
    uint64_t size;
};

struct ReceiveListDescriptor {
    uint64_t address;
    uint64_t size;
};

using SendDescriptor = RegularDescriptor;
using RecvDescriptor = RegularDescriptor;
using TransDescriptor = RegularDescriptor;
using StaticInDescriptor = PointerDescriptor;
using StaticOutDescriptor = ReceiveListDescriptor;

struct IpcInfo {
    uint64_t pid;
    uint32_t cmdId;
    std::vector<SendDescriptor> sendDescriptors; // A
    std::vector<RecvDescriptor> recvDescriptors; // B
    std::vector<TransDescriptor> transDescriptors; // W
    std::vector<StaticInDescriptor> staticInDescriptors; // X
    std::vector<StaticOutDescriptor> staticOutDescriptors; // C
    std::vector<handle_t> copiedHandles;
    std::vector<handle_t> movedHandles;
    std::vector<uint8_t> argData;
    // TODO fully flesh this out
    uint64_t rawData;
    uint64_t buffer;
    bool isDomain;
    uint8_t domainCmd;
    uint16_t domainSize;
    uint32_t domainObjId; // handle_t?
};

uint64_t doIpcCall(handle_t handle, uint64_t buffer, uint64_t size);

// TODO assert argData.size() >= sizeof(T)
template<typename T>
void inline readArgs(IpcInfo* info, T* args) { *args = *reinterpret_cast<T*>(info->argData.data()); };

class IpcMessageCons {
    // TODO buffers?
public:
    IpcMessageCons(bool isReply_, uint16_t type_, IpcInfo* info) : isReply(isReply_), isDomain(info->isDomain), memOffset(info->buffer), type(type_) {}

    // TODO phase out
    void writeData(uint64_t data) {
        for (auto i = 0; i < 8; i++) {
            dataBuf.push_back(static_cast<uint8_t>((data >> (8 * i)) & 0xFF));
        }
    }
    void writeError(uint64_t err) { error = err; }
    void writeCommandId(uint64_t id) { command = id; }

    template<typename T>
    void inline writeArgs(T* args) {
        for (auto i = 0; i < sizeof(*args); i++) {
            dataBuf.push_back(*(reinterpret_cast<uint8_t*>(args) + i));
        }
    }

    void copyHandle(handle_t handle) { copiedHandles.push_back(handle); }
    void moveHandle(handle_t handle) { movedHandles.push_back(handle); }

    void construct();

    ~IpcMessageCons() {}

private:
    uint64_t memOffset;
    bool isReply;
    bool isDomain;
    uint16_t type;
    std::vector<uint8_t> dataBuf;
    std::vector<handle_t> copiedHandles;
    std::vector<handle_t> movedHandles;
    union {
        uint64_t error;
        uint64_t command;
    };
};

} // namespace ipc