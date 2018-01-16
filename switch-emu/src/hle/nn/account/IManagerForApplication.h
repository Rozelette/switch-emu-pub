#pragma once
#include "hle\service.h"

namespace nn {

namespace account {

namespace baas {

class IManagerForApplication : public service::Interface {

    SERVICE_FUNC_STUB(CheckAvailability);
    SERVICE_FUNC_STUB(GetAccountId);
    SERVICE_FUNC_STUB(EnsureIdTokenCacheAsync);
    SERVICE_FUNC_STUB(LoadIdTokenCache);
    SERVICE_FUNC_STUB(GetNintendoAccountUserResourceCacheForApplication);
    SERVICE_FUNC_STUB(CreateAuthorizationRequest);

public:
    IManagerForApplication() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IManagerForApplication::CheckAvailability), "CheckAvailability" },
            { 1, static_cast<InterfaceFunc>(&IManagerForApplication::GetAccountId), "GetAccountId" },
            { 2, static_cast<InterfaceFunc>(&IManagerForApplication::EnsureIdTokenCacheAsync), "EnsureIdTokenCacheAsync" },
            { 3, static_cast<InterfaceFunc>(&IManagerForApplication::LoadIdTokenCache), "LoadIdTokenCache" },
            { 130, static_cast<InterfaceFunc>(&IManagerForApplication::GetNintendoAccountUserResourceCacheForApplication), "GetNintendoAccountUserResourceCacheForApplication" },
            { 150, static_cast<InterfaceFunc>(&IManagerForApplication::CreateAuthorizationRequest), "CreateAuthorizationRequest" },
        });
    }

    std::string getName() {
        return "nn::account::baas::IManagerForApplication";
    }
};

} // namespace baas

} // namespace account

} // namespace nn
