#include "nvhostCtrlGpuInterface.h"
#include "log.h"

namespace gpu {

uint32_t NvhostCtrlGpuInterface::ZcullGetCtxSize(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    ZcullGetCtxSizeArgs objectOut;

    //objectOut.size = 0x16; // from gr_fecs_method_push_adr_discover_zcull_image_size_v() in \kernel\nvgpu\drivers\gpu\nvgpu\include\nvgpu\hw\gm20b\hw_gr_gm20b.h
    objectOut.size = 0x10200; // from https://github.com/reswitched/Mephisto/pull/25

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvhostCtrlGpuInterface::ZcullGetInfo(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    ZcullGetInfoArgs objectOut; // default constructor handles initializing values

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvhostCtrlGpuInterface::GetCharacteristics(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    GetCharacteristicsArgs objectIn, objectOut;
    readObjectFromBuffer(objectIn, in);

    objectOut.addr = objectIn.addr; // TODO do we write the object to this addr as well?
    objectOut.size = objectIn.size;
    // default constructor handles setting all the gpu characteristics

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvhostCtrlGpuInterface::GetTpcMasks(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    GetTpcMasksArgs objectIn, objectOut;
    readObjectFromBuffer(objectIn, in);

    objectOut.bufSize = 4;
    objectOut.bufAddr = objectIn.bufAddr;
    objectOut.unknown_tpcMasks = 0x3; // TODO verify on hw. Seems to be a bit per enabled tpc (of which there are two)
                                      // See \kernel\nvgpu\drivers\gpu\nvgpu\gm20b\gr_gm20b.c in linux driver

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvhostCtrlGpuInterface::GetL2State(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    GetL2StateArgs  objectOut;

    objectOut.mask = 0x7;
    objectOut.flush = 0x0;

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

} // namespace gpu