#pragma once
#include "hle\service.h"

// NS Services
namespace nn {

namespace aocsrv {

namespace detail {

class IAddOnContentManager : public service::Interface {

    SERVICE_FUNC_STUB(CountAddOnContentByApplicationId);
    SERVICE_FUNC_STUB(ListAddOnContentByApplicationId);
    SERVICE_FUNC_STUB(CountAddOnContent);
    SERVICE_FUNC_STUB(ListAddOnContent);
    SERVICE_FUNC_STUB(GetAddOnContentBaseIdByApplicationId);
    SERVICE_FUNC_STUB(GetAddOnContentBaseId);
    SERVICE_FUNC_STUB(PrepareAddOnContentByApplicationId);
    SERVICE_FUNC_STUB(PrepareAddOnContent);

public:
    IAddOnContentManager() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IAddOnContentManager::CountAddOnContentByApplicationId), "CountAddOnContentByApplicationId" },
            { 1, static_cast<InterfaceFunc>(&IAddOnContentManager::ListAddOnContentByApplicationId), "ListAddOnContentByApplicationId" },
            { 2, static_cast<InterfaceFunc>(&IAddOnContentManager::CountAddOnContent), "CountAddOnContent" },
            { 3, static_cast<InterfaceFunc>(&IAddOnContentManager::ListAddOnContent), "ListAddOnContent" },
            { 4, static_cast<InterfaceFunc>(&IAddOnContentManager::GetAddOnContentBaseIdByApplicationId), "GetAddOnContentBaseIdByApplicationId" },
            { 5, static_cast<InterfaceFunc>(&IAddOnContentManager::GetAddOnContentBaseId), "GetAddOnContentBaseId" },
            { 6, static_cast<InterfaceFunc>(&IAddOnContentManager::PrepareAddOnContentByApplicationId), "PrepareAddOnContentByApplicationId" },
            { 7, static_cast<InterfaceFunc>(&IAddOnContentManager::PrepareAddOnContent), "PrepareAddOnContent" },
        });

        addService(this, "aoc:u");
    }

    std::string getName() {
        return "nn::aocsrv::detail::IAddOnContentManager";
    }
};

} // namespace detail

} // namespace aocsrv

} // namespace nn
