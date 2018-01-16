#include "IApplicationProxyService.h"
#include "IApplicationProxy.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace am {

namespace service {

uint32_t IApplicationProxyService::OpenApplicationProxy(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* proxy = new IApplicationProxy();
    reply->moveHandle(proxy->connect());

    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn