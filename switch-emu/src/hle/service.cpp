#include <string>

#include "service.h"
#include "log.h"
#include "hle\kernel\handleManager.h"

#include "nn\lm\ILogService.h"
#include "nn\apm\IManager.h"
#include "nn\am\IApplicationProxyService.h"
#include "nn\am\IAllSystemAppletProxiesService.h"
#include "nn\aocsrv\IAddOnContentManager.h"
#include "nn\account\IAccountServiceForApplication.h"
#include "nn\pctl\IParentalControlServiceFactory.h"
#include "SmService\SmService.h"
#include "nn\settings\ISettingsServer.h"
#include "nn\timesrv\IStaticService.h"
#include "nn\fssrv\IFileSystemProxy.h"
#include "nn\hid\IHidServer.h"
#include "nns\nvdrv\INvDrvServices.h"
#include "nn\visrv\IManagerRootService.h"
#include "nn\visrv\ISystemRootService.h"
#include "nn\visrv\IApplicationRootService.h"
#include "nn\nifm\IStaticService.h"

namespace service {

void Interface::registerFunctions(const std::vector<FunctionEntry>& funcs) {
    mFunctions = funcs;
}

void Interface::handleSyncRequest(ipc::IpcInfo* info) {
    for (auto& func : mFunctions) {
        if ((func.id == info->cmdId) && (func.func != nullptr)) {
            ipc::IpcMessageCons reply(true, 0, info);
            logger::info("Program called %s.%s", getName().c_str(), func.name);
            uint32_t error = (this->*func.func)(info, &reply);
            reply.writeError(error);
            reply.construct();
            return;
        }
    }

    logger::error("Unknown command called for %s: %u", getName().c_str(), info->cmdId);
}

std::unordered_map<std::string, Interface*> gNamedPorts;

handle_t Interface::connect() {
    handle_t handle = kernel::handleman::createHandle(ObjectPtr<KAutoObject>(new InterfaceHandle(this, false)));
    logger::info("Connected to %s with handle %08X", this->getName().c_str(), handle);
    return handle;
}

void addNamedPort(Interface* interface, std::string name) {
    gNamedPorts.emplace(name, interface);
}

void addService(Interface* interface, std::string name) {
    SmService::internal::registerService(name, interface);
}

SmService* smModule;
nn::pctl::detail::ipc::IParentalControlServiceFactory* pctlModule;
nn::apm::IManager* apmModule;
nn::am::service::IApplicationProxyService* appletOEModule;
nn::am::service::IAllSystemAppletProxiesService* appletAEModule;
nn::aocsrv::detail::IAddOnContentManager* aocModule;
nn::account::IAccountServiceForApplication* accModule;
nn::lm::ILogService* lmModule;
nn::settings::ISettingsServer* setModule;
nn::timesrv::detail::service::IStaticService* timeModule;
nn::fssrv::sf::IFileSystemProxy* fspSrvModule;
nn::hid::IHidServer* hidModule;
nns::nvdrv::INvDrvServices* nvdrvModule;
nn::visrv::sf::IManagerRootService* vimModule;
nn::visrv::sf::ISystemRootService* visModule;
nn::visrv::sf::IApplicationRootService* viuModule;
nn::nifm::detail::IStaticService* nifmModule;

void init() {
    // TODO don't register services in constructor, and do it here instead?
    smModule = new SmService();

    apmModule = new nn::apm::IManager();
    appletOEModule = new nn::am::service::IApplicationProxyService();
    appletAEModule = new nn::am::service::IAllSystemAppletProxiesService();
    aocModule = new nn::aocsrv::detail::IAddOnContentManager();
    accModule = new nn::account::IAccountServiceForApplication();
    pctlModule = new nn::pctl::detail::ipc::IParentalControlServiceFactory();
    lmModule = new nn::lm::ILogService();
    setModule = new nn::settings::ISettingsServer();
    timeModule = new nn::timesrv::detail::service::IStaticService();
    fspSrvModule = new nn::fssrv::sf::IFileSystemProxy();
    hidModule = new nn::hid::IHidServer();
    nvdrvModule = new nns::nvdrv::INvDrvServices();
    vimModule = new nn::visrv::sf::IManagerRootService();
    visModule = new nn::visrv::sf::ISystemRootService();
    viuModule = new nn::visrv::sf::IApplicationRootService();
    nifmModule = new nn::nifm::detail::IStaticService();

    logger::info("Initialized services");
}

uint32_t connect() {

    return 0;
}

} // namespace service