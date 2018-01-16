#include "nvhostCtrlInterface.h"
#include "log.h"

namespace gpu {

uint32_t NvhostCtrlInterface::GetConfig(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    GetConfigArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command GetConfig");
    logger::info("GetConfigArgs");
    logger::info("domainStr: %s", objectIn.domainStr);
    logger::info("paramStr: %s", objectIn.paramStr);

    objectOut.domainStr[0] = '\0';
    objectOut.paramStr[0] = '\0';
    objectOut.configStr[0] = '\0';

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS; // not supported on retail?
}

} // namespace gpu