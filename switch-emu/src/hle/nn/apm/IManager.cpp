#include "IManager.h"
#include "ISession.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace apm {

uint32_t IManager::OpenSession(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* session = new ISession();
    reply->moveHandle(session->connect());

    return ERR_SUCCESS;
}

} // namespace apm

} // namespace nn