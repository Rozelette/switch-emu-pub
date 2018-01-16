#include <fstream>
#include <vector>

#include "IStorage.h"
#include "hle\ipc.h"
#include "log.h"
#include "mem.h"
#include "hle\kernel\kernel.h"

namespace nn {

namespace fssrv {

namespace sf {

uint32_t IStorage::Read(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    // TODO two args, offset & size?
    uint64_t addr = info->recvDescriptors[0].address;
    uint64_t size = info->recvDescriptors[0].size;

    uint64_t offset = mem::read<uint64_t>(info->rawData + 16);
    uint64_t size2 = mem::read<uint64_t>(info->rawData + 24);

    logger::info("IStorage_fs %s read to %016llX of size %016llX, offset = %016llX, size2 = %016llX", name.c_str(), addr, size, offset, size2);

    // TODO better file handling
    std::string path = kernel::getGameCartPath();
    path += "romfs";
    std::ifstream fileStream(path, std::ifstream::binary);
    if (!fileStream.is_open()) {
        logger::error("Couldn't open file %s", path.c_str());
    } else {
        fileStream.seekg(offset);
        std::vector<char> buffer(size);
        fileStream.read(buffer.data(), size);

        for (uint64_t i = 0; i < size; i++) {
            mem::write<uint8_t>(addr + i, static_cast<uint8_t>(buffer[i]));
        }
    }

    return ERR_SUCCESS;
}

} // namespace sf

} // namespace fssrv

} // namespace nn