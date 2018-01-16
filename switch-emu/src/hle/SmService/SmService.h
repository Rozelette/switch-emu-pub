#pragma once
#include "hle\service.h"

class SmService : public service::Interface {

    bool isInitialized = false;

    static const uint32_t ERR_NOT_INITIALIZED = ERR_CONS(ERR_MODULE_SM, 0x4);
    static const uint32_t ERR_MAX_SESSIONS = ERR_CONS(ERR_MODULE_SM, 0x6);
    static const uint32_t ERR_INVALID_NAME = ERR_CONS(ERR_MODULE_SM, 0xC);
    static const uint32_t ERR_PERMISSION_DENIED = ERR_CONS(ERR_MODULE_SM, 0x10);

    uint32_t Initialize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t GetService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t RegisterService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);
    uint32_t UnregisterService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply);

public:
    SmService() {
        registerFunctions({
            { 0, static_cast<InterfaceFunc>(&SmService::Initialize), "Initialize" },
            { 1, static_cast<InterfaceFunc>(&SmService::GetService), "GetService" },
            { 2, static_cast<InterfaceFunc>(&SmService::RegisterService), "RegisterService" },
            { 3, static_cast<InterfaceFunc>(&SmService::UnregisterService), "UnregisterService" },
        });

        addNamedPort(this, "sm:");
    }

    std::string getName() {
        return "SmService";
    }

    class internal {
    public:
        static void registerService(std::string name, service::Interface* interface);
    };
};