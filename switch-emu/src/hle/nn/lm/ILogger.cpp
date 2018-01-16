#include <vector>

#include "ILogger.h"
#include "hle\ipc.h"
#include "log.h"
#include "mem.h"
#include "cpu.h"

namespace nn {

namespace lm {

uint32_t ILogger::unk0(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) {
    // TODO
    uint64_t addr = info->staticInDescriptors[0].address;
    uint64_t size = info->staticInDescriptors[0].size;
    std::vector<uint8_t> dataBuf(size);
    for (uint64_t i = 0; i < size; i++) {
        dataBuf[i] = static_cast<char>(mem::read<uint8_t>(addr + i));
    }

    // TODO better way to get this data
    void* data = reinterpret_cast<void*>(dataBuf.data());

    // data + 0x0: (u64?) unk (0?)
    // data + 0x8: ptr to ThreadContext?
    // data + 0x10: (u32?) unk
    // data + 0x14: (u32?) msg size (= staticInDescriptors.address - 0x18)
    // data + 0x18: msg, consists of a list of strings? (when dataType == 3 or 6, seems to not be a string TODO document):
    //    header:
    //        u8: dataType?
    //        u8: length
    //    text: char[length] (no terminator)

    struct Header {
        uint64_t unk64;
        uint64_t threadContextPtr;
        uint32_t unk32;
        uint32_t msgSize;
    } header;
    header = *reinterpret_cast<Header*>(data);

    logger::info("ILogger::unk0:");
    logger::info("unk64: 0x%016llX", header.unk64);
    logger::info("threadContextPtr: 0x%016llX", header.threadContextPtr);
    logger::info("unk32: 0x%08X", header.unk32);
    logger::info("msgSize: 0x%08X", header.msgSize);

    for (auto i = 0u; i < header.msgSize;) {
        uint8_t dataType = reinterpret_cast<uint8_t*>(data)[i++ + 0x18];
        uint8_t strLength = reinterpret_cast<uint8_t*>(data)[i++ + 0x18];
        std::string s = "";
        for (auto charOffset = 0u; charOffset < strLength; charOffset++) {
            s += reinterpret_cast<uint8_t*>(data)[charOffset + i + 0x18];
        }
        s += "\0";

        logger::info("String(%d): %s", dataType, s.c_str());

        i += strLength;
    }

    cpu::sendInterrupts(0, cpu::DEBUG_BREAK_INTERRUPT);

    return ERR_SUCCESS;
}

} // namespace lm

} // namespace nn