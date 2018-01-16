#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class IDebugFunctions : public ::service::Interface {

    SERVICE_FUNC_STUB(NotifyMessageToHomeMenuForDebug);
    SERVICE_FUNC_STUB(OpenMainApplication);
    SERVICE_FUNC_STUB(EmulateButtonEvent);
    SERVICE_FUNC_STUB(InvalidateTransitionLayer);

public:
    IDebugFunctions() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IDebugFunctions::NotifyMessageToHomeMenuForDebug), "NotifyMessageToHomeMenuForDebug" },
            { 1, static_cast<InterfaceFunc>(&IDebugFunctions::OpenMainApplication), "OpenMainApplication" },
            { 10, static_cast<InterfaceFunc>(&IDebugFunctions::EmulateButtonEvent), "EmulateButtonEvent" },
            { 20, static_cast<InterfaceFunc>(&IDebugFunctions::InvalidateTransitionLayer), "InvalidateTransitionLayer" },
        });
    }

    std::string getName() {
        return "nn::am::service::IDebugFunctions";
    }
};

} // namespace service

} // namespace am

} // namespace nn
