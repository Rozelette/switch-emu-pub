#include "IProfile.h"
#include "hle\nn\account\profile.h"
#include "hle\ipc.h"
#include "log.h"
#include "mem.h"

namespace nn {

namespace account {

namespace profile {

uint32_t IProfile::GetBase(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::account::profile::ProfileBase profileBase;
    } out;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

} // namespace profile

} // namespace account

} // namespace nn