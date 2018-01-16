#include <sstream>
# include <iomanip>

#include "memDump.h"
#include "mem.h"
#include "log.h"

namespace debug {

void dumpMem(uint64_t addr, uint64_t size) {
    uint64_t startAddr = addr & ~0xF;
    uint64_t endAddr = (addr + size + 0xF) & ~0xF; // TODO align-up macro, TODO this could overflow

    for (uint64_t printAddr = startAddr; printAddr < endAddr; printAddr += 16) {
        char sBuf[17];
        sBuf[16] = '\0';

        std::stringstream ss;

        ss << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << printAddr << ": ";

        for (uint64_t col = 0; col < 16; col++) {
            uint8_t val = mem::inspect<uint8_t>(printAddr + col);
            sBuf[col] = ((val > 31) && (val < 127)) ? val : '.';
            if (((printAddr + col) < addr) || ((printAddr + col) >= (addr + size))) {
                ss << "XX";
            } else {
                ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<uint32_t>(val);
            }

            if (col == 7) {
                ss << ' ';
            }
        }

        ss << " | " << sBuf;

        logger::info("%s", ss.str().c_str());
    }
}

} // namespace debug