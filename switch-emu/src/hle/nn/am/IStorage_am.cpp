#include "IStorage.h"
#include "IStorageAccessor.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace am {

namespace service {

uint32_t IStorage::Open(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* storageAccessor = new IStorageAccessor();
    reply->moveHandle(storageAccessor->connect());

    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn