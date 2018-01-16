#include "IAppletResource.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace hid {

uint32_t IAppletResource::GetSharedMemoryHandle(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    reply->moveHandle(0x1234); // TODO actual handle

    return ERR_SUCCESS;
}

} // namespace hid

} // namespace nn