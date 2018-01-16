#pragma once
#include "hle\service.h"

// Display Services
namespace nn {

namespace visrv {

namespace sf {

class IApplicationRootService : public service::Interface {

    SERVICE_FUNC_STUB(GetDisplayService);

public:
    IApplicationRootService() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IApplicationRootService::GetDisplayService), "GetDisplayService" },
         });

        addService(this, "vi:u");
    }

    std::string getName() {
        return "nn::visrv::sf::IApplicationRootService";
    }
};

} // namespace sf

} // namespace visrv

} // namespace nn