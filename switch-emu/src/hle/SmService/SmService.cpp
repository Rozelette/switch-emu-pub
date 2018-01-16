#include <map>

#include "SmService.h"
#include "hle\ipc.h"
#include "log.h"

std::map<std::string, service::Interface*> gRegisteredServices;

uint32_t SmService::Initialize(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint64_t reserved;
    } args;
    ipc::readArgs(info, &args);

    if (args.reserved != 0) {
        logger::warn("SM::Initialize: reserved != 0");
    }

    if (isInitialized) {
        logger::warn("SM::Initialize: already initialized");
    }

    isInitialized = true;

    return ERR_SUCCESS;
}

uint32_t SmService::GetService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    struct {
        char name[8];
    } args;
    ipc::readArgs(info, &args);

    if (!isInitialized) {
        // returns this error in 3.0.1. Let's target 3.0.0 for now
        //return ERR_NOT_INITIALIZED;
    }

    // add trailing null as a name might use all 8 chars
    char nameBuffer[9];
    memcpy(nameBuffer, args.name, 8);
    nameBuffer[8] = '\0';
    std::string name(nameBuffer);
    logger::info("Game connected to service %s", name.c_str());

    // TODO permissions?
    Interface* service = gRegisteredServices[name];

    if (service == nullptr) {
        logger::error("Unkown service %s", name.c_str());
        return 1; // TODO actual error (ERR_INVALID_NAME?)
    } else {
        // TODO implement maxSessions
        handle_t handle = service->connect();
        reply->moveHandle(handle);
    }


    return ERR_SUCCESS;
}

uint32_t SmService::RegisterService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        char name[8];
        uint32_t isLight;
        uint32_t maxSessions;
    } args;
    ipc::readArgs(info, &args);

    // add trailing null as a name might use all 8 chars
    char nameBuffer[9];
    memcpy(nameBuffer, args.name, 8);
    nameBuffer[8] = '\0';
    std::string name(nameBuffer);
    logger::info("Game registered %s", name.c_str());

    return ERR_SUCCESS;
}

uint32_t SmService::UnregisterService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        char name[8];
        // TODO reserved uint64_t?
    } args;
    ipc::readArgs(info, &args);

    // add trailing null as a name might use all 8 chars
    char nameBuffer[9];
    memcpy(nameBuffer, args.name, 8);
    nameBuffer[8] = '\0';
    std::string name(nameBuffer);
    logger::info("Game unregistered %s", name.c_str());

    return ERR_SUCCESS;
}

void SmService::internal::registerService(std::string name, service::Interface* interface) {
    gRegisteredServices[name] = interface;
}