#include "ILibraryAppletProxy.h"
#include "ISelfController.h"
#include "IWindowController.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace am {

namespace service {

uint32_t ILibraryAppletProxy::GetSelfController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new ISelfController();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

uint32_t ILibraryAppletProxy::GetWindowController(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new IWindowController();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn