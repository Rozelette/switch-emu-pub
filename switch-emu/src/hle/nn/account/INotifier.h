#pragma once
#include "hle\service.h"

namespace nn {

namespace account {

namespace detail {

class INotifier : public service::Interface {

    SERVICE_FUNC_STUB(GetSystemEvent);

public:
    INotifier() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&INotifier::GetSystemEvent), "GetSystemEvent" },
        });
    }

    std::string getName() {
        return "nn::account::detail::INotifier";
    }
};

} // namespace detail

} // namespace account

} // namespace nn
