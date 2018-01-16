#pragma once
#include "IWindowController.h"
#include "hle\nn\applet\applet.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace am {

namespace service {

uint32_t IWindowController::GetAppletResourceUserId(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::applet::AppletResourceUserId id;
    } out;

    out.id = 0x12345678;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t IWindowController::AcquireForegroundRights(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn