#pragma once
#include "ICommonStateGetter.h"
#include "hle\nn\am\am.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace am {

namespace service {

// game seems to call this, then ReceiveMessage
// does this initiate a request to get data, then return a waitable/event that when signaled, means the data can be recieved with ReceiveMessage?
uint32_t ICommonStateGetter::GetEventHandle(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    reply->copyHandle(0x12345678); // some handle for a waitable/event?

    return ERR_SUCCESS;
}

uint32_t ICommonStateGetter::ReceiveMessage(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::am::AppletMessage message;
    } out;

    // game loops while the returned values != 0xF?
    // debug binaries expect this to return 1?
    // XXX alternate to satisfy both
    // TODO figure out what these messages mean and why debug and actual games differ (maybe it was just changed?)
    static nn::am::AppletMessage message = 0x1;
    out.message = message;
    if (message == 0x1) {
        message = 0xF;
    } else {
        message = 0x1;
    }

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t ICommonStateGetter::GetOperationMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint8_t unk; // a bool? enum?
    } out;

    out.unk = 0x15;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t ICommonStateGetter::GetPerformanceMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint32_t unk; // TODO find legal values
    } out;

    out.unk = 0x15;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t ICommonStateGetter::GetCurrentFocusState(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint8_t unk; // a bool? enum?
    } out;

    out.unk = 0x1;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn