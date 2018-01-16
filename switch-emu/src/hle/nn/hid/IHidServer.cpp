#include "IHidServer.h"
#include "IAppletResource.h"
#include "hle\nn\applet\applet.h"
#include "hle\nn\hid\hid.h"
#include "hle\ipc.h"
#include "log.h"

namespace nn {

namespace hid {

uint32_t IHidServer::CreateAppletResource(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::applet::AppletResourceUserId id;
    } args;

    ipc::readArgs(info, &args);

    logger::info("id: %016llX", args.id);

    Interface* appletResource = new IAppletResource();
    reply->moveHandle(appletResource->connect());

    return ERR_SUCCESS;
}

uint32_t IHidServer::ActivateTouchScreen(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::applet::AppletResourceUserId id;
    } args;

    ipc::readArgs(info, &args);

    logger::info("id: %016llX", args.id);

    return ERR_SUCCESS;
}

uint32_t IHidServer::SetSupportedNpadStyleSet(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::hid::NpadStyleTag styleTag;
        nn::applet::AppletResourceUserId id;
    } args;

    ipc::readArgs(info, &args);

    logger::info("styleTag: %08X", args.styleTag);
    logger::info("id: %016llX", args.id);

    return ERR_SUCCESS;
}

uint32_t IHidServer::SetSupportedNpadIdType(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::applet::AppletResourceUserId id;
    } args;

    ipc::readArgs(info, &args);

    logger::info("id: %016llX", args.id);

    return ERR_SUCCESS;
}

uint32_t IHidServer::ActivateNpad(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::applet::AppletResourceUserId id;
    } args;

    ipc::readArgs(info, &args);

    logger::info("id: %016llX", args.id);

    return ERR_SUCCESS;
}

uint32_t IHidServer::SetNpadJoyHoldType(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::applet::AppletResourceUserId id;
        int64_t type; // ?
    } args;

    ipc::readArgs(info, &args);

    logger::info("id: %016llX", args.id);
    logger::info("setting: %016llX", args.type);

    return ERR_SUCCESS;
}

} // namespace hid

} // namespace nn