#pragma once
#include "hle\service.h"

namespace nn {

namespace am {

namespace service {

class ILibraryAppletCreator : public ::service::Interface {

    SERVICE_FUNC_STUB(CreateLibraryApplet);
    SERVICE_FUNC_STUB(TerminateAllLibraryApplets);
    SERVICE_FUNC_STUB(AreAnyLibraryAppletsLeft);
    SERVICE_FUNC_STUB(CreateStorage);
    SERVICE_FUNC_STUB(CreateTransferMemoryStorage);
    SERVICE_FUNC_STUB(CreateHandleStorage);

public:
    ILibraryAppletCreator() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&ILibraryAppletCreator::CreateLibraryApplet), "CreateLibraryApplet" },
            { 1, static_cast<InterfaceFunc>(&ILibraryAppletCreator::TerminateAllLibraryApplets), "TerminateAllLibraryApplets" },
            { 2, static_cast<InterfaceFunc>(&ILibraryAppletCreator::AreAnyLibraryAppletsLeft), "AreAnyLibraryAppletsLeft" },
            { 10, static_cast<InterfaceFunc>(&ILibraryAppletCreator::CreateStorage), "CreateStorage" },
            { 11, static_cast<InterfaceFunc>(&ILibraryAppletCreator::CreateTransferMemoryStorage), "CreateTransferMemoryStorage" },
            { 12, static_cast<InterfaceFunc>(&ILibraryAppletCreator::CreateHandleStorage), "CreateHandleStorage" },
        });
    }

    std::string getName() {
        return "nn::am::service::ILibraryAppletCreator";
    }
};

} // namespace service

} // namespace am

} // namespace nn
