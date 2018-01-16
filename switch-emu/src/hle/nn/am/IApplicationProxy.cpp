#pragma once
#include "IApplicationProxy.h"
#include "ICommonStateGetter.h"
#include "ISelfController.h"
#include "IWindowController.h"
#include "IAudioController.h"
#include "IDisplayController.h"
#include "IProcessWindingController.h"
#include "ILibraryAppletCreator.h"
#include "IApplicationFunctions.h"
#include "IDebugFunctions.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace am {

namespace service {

uint32_t IApplicationProxy::GetCommonStateGetter(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new ICommonStateGetter();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationProxy::GetSelfController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new ISelfController();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationProxy::GetWindowController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new IWindowController();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationProxy::GetAudioController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new IAudioController();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationProxy::GetDisplayController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new IDisplayController();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationProxy::GetProcessWindingController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new IProcessWindingController();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationProxy::GetLibraryAppletCreator(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new ILibraryAppletCreator();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationProxy::GetApplicationFunctions(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new IApplicationFunctions();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationProxy::GetDebugFunctions(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new IDebugFunctions();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn