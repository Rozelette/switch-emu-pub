#include "IAllSystemAppletProxiesService.h"
#include "ILibraryAppletProxy.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace am {

namespace service {

uint32_t IAllSystemAppletProxiesService::OpenLibraryAppletProxyOld(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new ILibraryAppletProxy();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn