#pragma once
#include "hle\service.h"

namespace nn {

namespace settings {

class ISettingsServer : public service::Interface {

    SERVICE_FUNC_STUB(GetLanguageCode);
    uint32_t GetAvailableLanguageCodes(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetAvailableLanguageCodeCount);
    SERVICE_FUNC_STUB(GetRegionCode);

public:
    ISettingsServer() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ISettingsServer::GetLanguageCode), "GetLanguageCode" },
            { 1, static_cast<InterfaceFunc>(&ISettingsServer::GetAvailableLanguageCodes), "GetAvailableLanguageCodes" },
            { 3, static_cast<InterfaceFunc>(&ISettingsServer::GetAvailableLanguageCodeCount), "GetAvailableLanguageCodeCount" },
            { 4, static_cast<InterfaceFunc>(&ISettingsServer::GetRegionCode), "GetRegionCode" },
        });

        addService(this, "set");
    }

    std::string getName() {
        return "nn::settings::ISettingsServer";
    }
};

} // namespace settings

} // namespace nn
