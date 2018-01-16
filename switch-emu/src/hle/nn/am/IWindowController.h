#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class IWindowController : public ::service::Interface {

    SERVICE_FUNC_STUB(CreateWindow);
    uint32_t GetAppletResourceUserId(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t AcquireForegroundRights(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(ReleaseForegroundRights);
    SERVICE_FUNC_STUB(RejectToChangeIntoBackground);

public:
    IWindowController() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IWindowController::CreateWindow), "CreateWindow" },
            { 1, static_cast<InterfaceFunc>(&IWindowController::GetAppletResourceUserId), "GetAppletResourceUserId" },
            { 10, static_cast<InterfaceFunc>(&IWindowController::AcquireForegroundRights), "AcquireForegroundRights" },
            { 11, static_cast<InterfaceFunc>(&IWindowController::ReleaseForegroundRights), "ReleaseForegroundRights" },
            { 12, static_cast<InterfaceFunc>(&IWindowController::RejectToChangeIntoBackground), "RejectToChangeIntoBackground" },
        });
    }

    std::string getName() {
        return "nn::am::service::IWindowController";
    }
};

} // namespace service

} // namespace am

} // namespace nn
