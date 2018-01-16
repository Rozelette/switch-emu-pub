#pragma once
#include "hle\service.h"

namespace nn {

namespace account {

namespace profile {

class IProfile : public service::Interface {

    SERVICE_FUNC_STUB(Get);
    uint32_t GetBase(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetImageSize);
    SERVICE_FUNC_STUB(LoadImage);

public:
    IProfile() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IProfile::Get), "Get" },
            { 1, static_cast<InterfaceFunc>(&IProfile::GetBase), "GetBase" },
            { 10, static_cast<InterfaceFunc>(&IProfile::GetImageSize), "GetImageSize" },
            { 11, static_cast<InterfaceFunc>(&IProfile::LoadImage), "LoadImage" },
        });
    }

    std::string getName() {
        return "nn::account::profile::IProfile";
    }
};

} // namespace profile

} // namespace account

} // namespace nn
