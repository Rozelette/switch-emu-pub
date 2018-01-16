#pragma once
#include "hle\service.h"

// Network Interface services
namespace nn {

namespace nifm {

namespace detail {

class IStaticService : public service::Interface {

    uint32_t CreateGeneralServiceOld(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(CreateGeneralService);

public:
    IStaticService() {
        registerFunctions({
            { 4, static_cast<InterfaceFunc>(&IStaticService::CreateGeneralServiceOld), "CreateGeneralServiceOld" },
            { 5, static_cast<InterfaceFunc>(&IStaticService::CreateGeneralService), "CreateGeneralService" },
        });

        // TODO how are these different?
        addService(this, "nifm:u");
        addService(this, "nifm:a");
        addService(this, "nifm:s");
    }

    std::string getName() {
        return "nn::nifm::detail::IStaticService";
    }
};

} // namespace detail

} // namespace nifm

} // namespace nn
