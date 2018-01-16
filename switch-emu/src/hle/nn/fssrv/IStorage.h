#pragma once
#include "hle\service.h"

namespace nn {

namespace fssrv {

namespace sf {

class IStorage : public service::Interface {

    uint32_t Read(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    SERVICE_FUNC_STUB(Write);
    SERVICE_FUNC_STUB(Flush);
    SERVICE_FUNC_STUB(SetSize);
    SERVICE_FUNC_STUB(GetSize);

public:
    IStorage(std::string name_) : name(name_) {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&IStorage::Read), "Read" },
            { 1, static_cast<InterfaceFunc>(&IStorage::Write), "Write" },
            { 2, static_cast<InterfaceFunc>(&IStorage::Flush), "Flush" },
            { 3, static_cast<InterfaceFunc>(&IStorage::SetSize), "SetSize" },
            { 4, static_cast<InterfaceFunc>(&IStorage::GetSize), "GetSize" },
        });
    }

    std::string getName() {
        return "nn::fssrv::sf::IStorage";
    }
private:
    //temp
    std::string name;
};

} // namespace sf

} // namespace fssrv

} // namespace nn
