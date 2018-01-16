#include "IAccountServiceForApplication.h"
#include "IManagerForApplication.h"
#include "IProfile.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace account {

uint32_t IAccountServiceForApplication::GetProfile(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    service::Interface* profile = new profile::IProfile();
    reply->moveHandle(profile->connect());

    return ERR_SUCCESS;
}

uint32_t IAccountServiceForApplication::InitializeApplicationInfo(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    return ERR_SUCCESS;
}

uint32_t IAccountServiceForApplication::GetBaasAccountManagerForApplication(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    service::Interface* notifier = new baas::IManagerForApplication();
    reply->moveHandle(notifier->connect());

    return ERR_SUCCESS;
}

} // namespace account

} // namespace nn