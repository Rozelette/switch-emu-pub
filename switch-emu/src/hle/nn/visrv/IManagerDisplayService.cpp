#include "IManagerDisplayService.h"
#include "hle\nn\applet\applet.h"
#include "hle\nn\vi\Display.h"

extern nn::vi::emu::DisplayManager gDisplayManager;

namespace nn {

namespace visrv {

namespace sf {

uint32_t IManagerDisplayService::CreateManagedLayer(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        int32_t unk;
        int32_t pad;
        int64_t displayId;
        nn::applet::AppletResourceUserId id;
    } args;

    ipc::readArgs(info, &args);

    uint64_t layerId = gDisplayManager.createLayer(args.displayId);

    struct {
        uint64_t layerId;
    } out;

    out.layerId = layerId;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t IManagerDisplayService::AddToLayerStack(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO

    return ERR_SUCCESS;
}

} // namespace sf

} // namespace visrv

} // namespace nn