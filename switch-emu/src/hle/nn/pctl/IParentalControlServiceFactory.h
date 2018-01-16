#pragma once
#include "hle\service.h"

// Parental Control services
namespace nn {

namespace pctl {

namespace detail {

namespace ipc {

class IParentalControlServiceFactory : public service::Interface {

    uint32_t CreateService(::ipc::IpcInfo* info, ::ipc::IpcMessageCons* reply);

public:
    IParentalControlServiceFactory() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IParentalControlServiceFactory::CreateService), "CreateService" },
        });

        // TODO how are these different?
        addService(this, "pctl");
        addService(this, "pctl:a");
        addService(this, "pctl:s");
        addService(this, "pctl:r");
    }

    std::string getName() {
        return "nn::pctl::detail::ipc::IParentalControlServiceFactory";
    }
};

} // namespace ipc

} // namespace detail

} // namespace pctl

} // namespace nn
