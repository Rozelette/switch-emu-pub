#pragma once
#include "IApplicationFunctions.h"
#include "hle\nn\settings\settings.h"
#include "IStorage.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace am {

namespace service {

uint32_t IApplicationFunctions::PopLaunchParameter(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* storage = new IStorage();
    reply->moveHandle(storage->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationFunctions::GetDesiredLanguage(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::settings::LanguageCode code;
    } out;

    out.code = 0x1;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t IApplicationFunctions::SetTerminateResult(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint32_t result;
    } args;

    ipc::readArgs(info, &args);

    logger::info("result: %08X", args.result);

    return ERR_SUCCESS;
}

uint32_t IApplicationFunctions::NotifyRunning(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        bool unk;
    } out;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn