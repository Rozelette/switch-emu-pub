#pragma once
#include "ISelfController.h"
#include "hle\nn\am\am.h"
#include "hle\ipc.h"
#include "log.h"
#include "hle\nn\vi\Display.h"

extern nn::vi::emu::DisplayManager gDisplayManager;

namespace nn {

namespace am {

namespace service {

uint32_t ISelfController::SetOperationModeChangedNotification(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        bool setting;
    } args;

    ipc::readArgs(info, &args);

    logger::info("setting: %u", args.setting);

    return ERR_SUCCESS;
}

uint32_t ISelfController::SetPerformanceModeChangedNotification(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        bool setting;
    } args;

    ipc::readArgs(info, &args);

    logger::info("setting: %u", args.setting);

    return ERR_SUCCESS;
}

uint32_t ISelfController::SetFocusHandlingMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        bool setting1;
        bool setting2;
        bool setting3;
    } args;

    ipc::readArgs(info, &args);

    logger::info("setting1: %u", args.setting1);
    logger::info("setting2: %u", args.setting2);
    logger::info("setting3: %u", args.setting3);

    return ERR_SUCCESS;
}

uint32_t ISelfController::SetRestartMessageEnabled(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        bool setting;
    } args;

    ipc::readArgs(info, &args);

    logger::info("setting: %u", args.setting);

    return ERR_SUCCESS;
}

uint32_t ISelfController::SetScreenShotAppletIdentityInfo(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::am::service::AppletIdentityInfo info;
    } args;

    ipc::readArgs(info, &args);

    return ERR_SUCCESS;
}

uint32_t ISelfController::SetOutOfFocusSuspendingEnabled(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        bool setting;
    } args;

    ipc::readArgs(info, &args);

    logger::info("setting: %u", args.setting);

    return ERR_SUCCESS;
}

uint32_t ISelfController::CreateManagedDisplayLayer(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint64_t layerId;
    } out;

    out.layerId = gDisplayManager.createLayer(gDisplayManager.openDisplay("Default"));

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

} // namespace service

} // namespace am

} // namespace nn