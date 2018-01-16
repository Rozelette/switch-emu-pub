#pragma once
#include "hle\service.h"

namespace nns {

namespace hosbinder {

class IHOSBinderDriver : public service::Interface {

    uint32_t TransactParcel(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t AdjustRefcount(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetNativeHandle(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t TransactParcelAuto(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);

public:
    IHOSBinderDriver() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IHOSBinderDriver::TransactParcel), "TransactParcel" },
            { 1, static_cast<InterfaceFunc>(&IHOSBinderDriver::AdjustRefcount), "AdjustRefcount" },
            { 2, static_cast<InterfaceFunc>(&IHOSBinderDriver::GetNativeHandle), "GetNativeHandle" },
            { 3, static_cast<InterfaceFunc>(&IHOSBinderDriver::TransactParcelAuto), "TransactParcelAuto" },
        });
    }

    std::string getName() {
        return "nns::hosbinder::IHOSBinderDriver";
    }

    // from https://github.com/reswitched/libtransistor/blob/master/lib/display/graphic_buffer_queue.c#L13
    enum class Transaction {
        REQUEST_BUFFER = 0x1,
        SET_BUFFER_COUNT = 0x2,
        DEQUEUE_BUFFER = 0x3,
        DETACH_BUFFER = 0x4,
        DETACH_NEXT_BUFFER = 0x5,
        ATTACH_BUFFER = 0x6,
        QUEUE_BUFFER = 0x7,
        CANCEL_BUFFER = 0x8,
        QUERY = 0x9,
        CONNECT = 0xA,
        DISCONNECT = 0xB,
        // SET_SIDEBAND_STREAM?
        ALLOCATE_BUFFERS = 0xD,
        SET_PREALLOCATED_BUFFER = 0xE
    };
};

} // namespace hosbinder

} // namespace nns
