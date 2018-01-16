#include "IApplicationDisplayService.h"
#include "IManagerDisplayService.h"
#include "ISystemDisplayService.h"
#include "hle\nns\hosbinder\IHOSBinderDriver.h"
#include "hle\nn\vi\vi.h"
#include "hle\nn\applet\applet.h"
#include "hle\ipc.h"
#include "log.h"
#include "mem.h"
#include "hle/nn/vi/Display.h"

extern nn::vi::emu::DisplayManager gDisplayManager;

namespace nn {

namespace visrv {

namespace sf {

uint32_t IApplicationDisplayService::GetRelayService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* ihosBinderDriver = new nns::hosbinder::IHOSBinderDriver();
    reply->moveHandle(ihosBinderDriver->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationDisplayService::GetSystemDisplayService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* systemDisplayService = new ISystemDisplayService();
    reply->moveHandle(systemDisplayService->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationDisplayService::GetManagerDisplayService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* managerDisplayService = new IManagerDisplayService();
    reply->moveHandle(managerDisplayService->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationDisplayService::GetIndirectDisplayTransactionService(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    Interface* ihosBinderDriver = new nns::hosbinder::IHOSBinderDriver();
    reply->moveHandle(ihosBinderDriver->connect());

    return ERR_SUCCESS;
}

uint32_t IApplicationDisplayService::OpenDisplay(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        nn::vi::DisplayName name; // TODO this might not be null-terminated
    } args;

    ipc::readArgs(info, &args);

    logger::info("name: %s", args.name);

    uint64_t id = gDisplayManager.openDisplay(args.name.name);

    struct {
        uint64_t displayId;
    } out;

    out.displayId = id;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t IApplicationDisplayService::OpenLayer(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    // This is a giant shameful hack
    // TODO
    struct {
        nn::vi::DisplayName name; // TODO this might not be null-terminated
        uint64_t layerId;
        nn::applet::AppletResourceUserId id;
    } args;

    ipc::readArgs(info, &args);

    logger::info("name: %s", args.name);
    logger::info("layerId: %016llX", args.layerId);
    logger::info("id: %016llX", args.id);

    uint64_t addr = info->recvDescriptors[0].address;
    uint64_t size = info->recvDescriptors[0].size;

    for (uint64_t i = 0; i < size; i++) {
        mem::write<uint8_t>(addr + i, 0);
    }

    mem::write<uint32_t>(addr + 0x0, 0x28);
    mem::write<uint32_t>(addr + 0x4, 0x10);
    mem::write<uint32_t>(addr + 0x8, 0x4); // can't be 0, otherwise fails assert in sdk
    mem::write<uint32_t>(addr + 0xC, 0xf0); // can't be 0, otherwise fails assert in sdk

    // NativeWindow?
    mem::write<uint32_t>(addr + 0x10 + 0x0, 2); // magic
    mem::write<uint32_t>(addr + 0x10 + 0x4, 0); // process_id
    mem::write<uint32_t>(addr + 0x10 + 0x8, gDisplayManager.getLayer(args.layerId).getBufferId()); // buffer_id
    // 0xC of padding?
    mem::write<uint8_t>(addr + 0x10 + 0xC + 0xC + 0x0, 'd');
    mem::write<uint8_t>(addr + 0x10 + 0xC + 0xC + 0x1, 's');
    mem::write<uint8_t>(addr + 0x10 + 0xC + 0xC + 0x2, 'p');
    mem::write<uint8_t>(addr + 0x10 + 0xC + 0xC + 0x3, 'd');
    mem::write<uint8_t>(addr + 0x10 + 0xC + 0xC + 0x4, 'r');
    mem::write<uint8_t>(addr + 0x10 + 0xC + 0xC + 0x5, 'v');
    // 0x8 of padding?

    reply->writeData(0x100); // TODO what's a good size?

    /*

    mem::write<uint32_t>(addr + 0, 0xE0);
    mem::write<uint32_t>(addr + 4, 0x10);
    mem::write<uint32_t>(addr + 8, 0x4);
    mem::write<uint32_t>(addr + 0xc, 0xf0);

    mem::write<uint32_t>(addr + 0xf0, 0x0);

    mem::write<uint32_t>(addr + 0x10, 0);
    mem::write<uint32_t>(addr + 0x20, 0x4000000);

    mem::allocateRegion(mem::getPageNum(0x4000000), 3);
    mem::protectRegion(mem::getPageNum(0x4000000), 3, mem::Permission::ALL);

    for (uint64_t i = 0; i < 0x100; i++) {
        mem::write<uint64_t>(0x4000000 + i * 8, i * 0x0101010101010101);
    }
    mem::write<uint64_t>(0x4000000, 0x4000500);

    for (uint64_t i = 0; i < 0x100; i++) {
        mem::write<uint64_t>(0x4000400 + i * 8, 0x4001000 + i * 4);
    }

    for (uint64_t i = 0; i < 0x100; i++) {
        mem::write<uint32_t>(0x4001000 + i * 4, 0xD65F03C0);
    }

    mem::write<uint64_t>(0x4000500 - 0x18, 0);

    mem::write<uint64_t>(0x4000000 + 1 * 8, 0x4002000);
    mem::write<uint32_t>(0x4002000 + 4, 1);

    reply->writeData(0x100); // seems to be the size of the output written (including header)
    // size is calculated by 0x10 + (u32 at header + 0x0) + (u32 at header + 0x8)

    */

    return ERR_SUCCESS;

    // notes:
    // writes to a B buffer of size 0x100 (is this size const or devired from a different return value?, probably const, pointer is to the stack)
    // seems to have a header of 0x10 bytes
    // header has 4 u32s
    // 0x0 : data size
    // 0x4 : data offset
    // 0x8 : objects offset array size - nn::vi::native::ParcelIo::OpenParcel calculates parameter objectsCount by dividing this by 4
    // 0xc : objects offset array offset
    // this is used by nn::vi::native::ParcelIo::OpenParcel to construct parameters for android::Parcel::ipcSetDataReference
    // see https://android.googlesource.com/platform/frameworks/native/+/jb-dev/include/binder/Parcel.h

    // objects seems to be a list of u32 offsets for flat_binder_object
    // offset i must not be < i * sizeof(flat_binder_object) (0x28 it seems, but online source seems to be 0x18...) (so in other words, they're sequencial)

    // flat_binder_object has a type at offset 0x0
    // if the type is 4 (BINDER_TYPE_FD?) a bool (mHasFds) is set

    // from the resulting Parcel, nn::vi::native::DeserializeNativeWindow passes it to android::Parcel::readStrongBinder, which is a wrapper for unflatten_binder
    // unflatten_binder reads in an object, then returns 0 iff type is BINDER_TYPE_BINDER (0) or BINDER_TYPE_HANDLE (2)
    // for BINDER_TYPE_BINDER, a pointer at offset 0x10 (cookie) is returned to out
    // for BINDER_TYPE_HANDLE, TODO (more complicated, lazy)

    // it seems like this is supposed to return an android::IGraphicBufferProducer. Does that mean that I will need to construct one in user space? That would really suck...
}

uint32_t IApplicationDisplayService::CreateStrayLayer(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    struct {
        uint32_t unk;
        uint32_t pad;
        uint64_t display;
    } args;

    ipc::readArgs(info, &args);

    logger::info("unk: %08X", args.unk);
    logger::info("pad: %08X", args.pad);
    logger::info("display: %016llX", args.display);

    uint64_t addr = info->recvDescriptors[0].address;
    uint64_t size = info->recvDescriptors[0].size; // should be 0x210?

    for (uint64_t i = 0; i < size; i++) {
        mem::write<uint8_t>(addr + i, 0);
    }

    mem::write<uint32_t>(addr + 0, 0x18); // data_size
    mem::write<uint32_t>(addr + 4, 0x10); // data_offset
    mem::write<uint32_t>(addr + 8, 0x0); // objects_size
    mem::write<uint32_t>(addr + 0xc, 0x28); // objects_offset

    // writes a flat_binder_object_t (see https://github.com/reswitched/libtransistor/blob/master/include/libtransistor/display/binder.h)
    // for now, let's only care about the handle
    Interface* ihosBinderDriver = new nns::hosbinder::IHOSBinderDriver();
    mem::write<uint32_t>(addr + 0x10 + 0x8, ihosBinderDriver->connect());

    struct {
        uint64_t layerId;
        uint64_t surfaceSize;
    } out;

    out.layerId = 1;
    out.surfaceSize = 0;

    reply->writeArgs(&out);

    return ERR_SUCCESS;
}

uint32_t IApplicationDisplayService::SetLayerScalingMode(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO

    return ERR_SUCCESS;
}

uint32_t IApplicationDisplayService::GetDisplayVsyncEvent(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    reply->copyHandle(0x12345678); // some handle for an event?

    return ERR_SUCCESS;
}

} // namespace sf

} // namespace visrv

} // namespace nn