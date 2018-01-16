#include <vector>
#include <algorithm>

#include "nvmapInterface.h"
#include "log.h"

namespace gpu {

std::vector<NvmapObject> gMapObjects; // TODO not be global

NvmapObject& lookupNvmapObject(uint32_t handle) {
    auto iter = std::find_if(gMapObjects.begin(), gMapObjects.end(), [&](const NvmapObject& obj) {return obj.handle == handle; });

    if (iter == gMapObjects.end()) {
        logger::error("Unknown Nvmap object handle %08X", handle);
    }
    return *iter;
}

uint32_t NvmapInterface::IocCreate(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    IocCreateArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command IocCreate");
    logger::info("IocCreateArgs");
    logger::info("size: %08X", objectIn.size);
    logger::info("handle: %08X", objectIn.handle);

    // TODO no
    static uint32_t idCounter = 0x1;
    static uint32_t handleCounter = 0x1;

    // TODO actual object
    NvmapObject newObject;
    newObject.handle = handleCounter++;
    newObject.id = idCounter++;
    newObject.size = objectIn.size;

    objectOut.size = objectIn.size;
    objectOut.handle = newObject.handle;

    gMapObjects.push_back(newObject);

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvmapInterface::IocFromId(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    IocFromIdArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command IocFromId");
    logger::info("IocFromIdArgs");
    logger::info("handle: %08X", objectIn.handle);
    logger::info("id: %08X", objectIn.id);

    objectOut.handle = objectIn.handle;
    for (NvmapObject& searchObject : gMapObjects) {
        if (searchObject.id == objectIn.id) {
            objectOut.handle = searchObject.handle;
            break;
        }
    }

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvmapInterface::IocAlloc(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    // TODO
    IocAllocArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command IocAlloc");
    logger::info("IocAllocArgs");
    logger::info("handle: %08X", objectIn.handle);
    logger::info("heapMask: %08X", objectIn.heapMask);
    logger::info("flags: %08X", objectIn.flags);
    logger::info("align: %08X", objectIn.align);
    logger::info("kind: %02X", objectIn.kind);
    logger::info("addr: %016X", objectIn.addr);

    for (NvmapObject& object : gMapObjects) {
        if (object.handle == objectIn.handle) {
            object.align = objectIn.align;
            object.heap = 0x40000000;
            object.kind = objectIn.kind;
            object.addr = objectIn.addr;
            break;
        }
    }

    objectOut.handle = objectIn.handle;
    objectOut.heapMask = objectIn.heapMask;
    objectOut.flags = objectIn.flags;
    objectOut.align = objectIn.align;
    objectOut.kind = 0;
    for (auto i = 0; i < 7; i++) { objectOut.pad[i] = objectIn.pad[i]; }
    //objectOut.addr = 0x0000000AAF800000; // XXX from svcCreateTransferMemory?
    //objectOut.addr = 0x0000000AA0035000; // libtransistor expects this?
    //objectOut.addr = 0x0000000AA00B2000;
    objectOut.addr = objectIn.addr;

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvmapInterface::IocParam(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    IocParamArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command IocParam");
    logger::info("IocParamArgs");
    logger::info("handle: %08X", objectIn.handle);
    logger::info("param: %08X", objectIn.param);
    logger::info("result: %08X", objectIn.result);

    uint32_t res = 0;
    NvmapObject object;
    // TODO better search
    for (NvmapObject searchObject : gMapObjects) {
        if (searchObject.handle == objectIn.handle) {
            object = searchObject;
            break;
        }
    }

    switch (objectIn.param) {
    case IocParamType::SIZE:
        res = object.size;
        break;
    case IocParamType::ALIGNMENT:
        res = object.align;
        break;
    case IocParamType::BASE:
        res = 0; // TODO error
        break;
    case IocParamType::HEAP:
        res = object.heap;
        break;
    case IocParamType::KIND:
        res = object.kind;
        break;
    case IocParamType::COMPR:
        res = 0; // TODO unused
        break;
    default:
        break;
    }

    objectOut.handle = objectIn.handle;
    objectOut.param = objectIn.param;
    objectOut.result = res;

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

uint32_t NvmapInterface::IocGetId(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    IocGetIdArgs objectIn, objectOut;

    readObjectFromBuffer(objectIn, in);

    logger::error("Unimplemented command IocGetId");
    logger::info("IocGetIdArgs");
    logger::info("handle: %08X", objectIn.handle);
    logger::info("id: %08X", objectIn.id);

    objectOut.handle = objectIn.handle;
    for (NvmapObject& searchObject : gMapObjects) {
        if (searchObject.handle == objectIn.handle) {
            objectOut.id = searchObject.id;
            break;
        }
    }

    writeObjectToBuffer(objectOut, out);

    return ERR_SUCCESS;
}

} // namespace gpu