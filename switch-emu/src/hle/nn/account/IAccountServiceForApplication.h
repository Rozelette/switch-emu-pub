#pragma once
#include "hle\service.h"

namespace nn {

namespace account {

class IAccountServiceForApplication : public service::Interface {

    SERVICE_FUNC_STUB(GetUserCount);
    SERVICE_FUNC_STUB(GetUserExistence);
    SERVICE_FUNC_STUB(ListAllUsers);
    SERVICE_FUNC_STUB(ListOpenUsers);
    SERVICE_FUNC_STUB(GetLastOpenedUser);
    uint32_t GetProfile(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(GetProfileDigest);
    SERVICE_FUNC_STUB(IsUserRegistrationRequestPermitted);
    SERVICE_FUNC_STUB(TrySelectUserWithoutInteraction);
    uint32_t InitializeApplicationInfo(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetBaasAccountManagerForApplication(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(AuthenticateApplicationAsync);
    SERVICE_FUNC_STUB(StoreSaveDataThumbnail);
    SERVICE_FUNC_STUB(ClearSaveDataThumbnail);
    SERVICE_FUNC_STUB(CreateGuestLoginRequest);

public:
    IAccountServiceForApplication() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IAccountServiceForApplication::GetUserCount), "GetUserCount" },
            { 1, static_cast<InterfaceFunc>(&IAccountServiceForApplication::GetUserExistence), "GetUserExistence" },
            { 2, static_cast<InterfaceFunc>(&IAccountServiceForApplication::ListAllUsers), "ListAllUsers" },
            { 3, static_cast<InterfaceFunc>(&IAccountServiceForApplication::ListOpenUsers), "ListOpenUsers" },
            { 4, static_cast<InterfaceFunc>(&IAccountServiceForApplication::GetLastOpenedUser), "GetLastOpenedUser" },
            { 5, static_cast<InterfaceFunc>(&IAccountServiceForApplication::GetProfile), "GetProfile" },
            { 6, static_cast<InterfaceFunc>(&IAccountServiceForApplication::GetProfileDigest), "GetProfileDigest" },
            { 50, static_cast<InterfaceFunc>(&IAccountServiceForApplication::IsUserRegistrationRequestPermitted), "IsUserRegistrationRequestPermitted" },
            { 51, static_cast<InterfaceFunc>(&IAccountServiceForApplication::TrySelectUserWithoutInteraction), "TrySelectUserWithoutInteraction" },
            { 100, static_cast<InterfaceFunc>(&IAccountServiceForApplication::InitializeApplicationInfo), "InitializeApplicationInfo" },
            { 101, static_cast<InterfaceFunc>(&IAccountServiceForApplication::GetBaasAccountManagerForApplication), "GetBaasAccountManagerForApplication" },
            { 102, static_cast<InterfaceFunc>(&IAccountServiceForApplication::AuthenticateApplicationAsync), "AuthenticateApplicationAsync" },
            { 110, static_cast<InterfaceFunc>(&IAccountServiceForApplication::StoreSaveDataThumbnail), "StoreSaveDataThumbnail" },
            { 111, static_cast<InterfaceFunc>(&IAccountServiceForApplication::ClearSaveDataThumbnail), "ClearSaveDataThumbnail" },
            { 120, static_cast<InterfaceFunc>(&IAccountServiceForApplication::CreateGuestLoginRequest), "CreateGuestLoginRequest" },
        });

        addService(this, "acc:u0");
    }

    std::string getName() {
        return "nn::account::IAccountServiceForApplication";
    }
};

} // namespace account

} // namespace nn
