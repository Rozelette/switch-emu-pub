#include "emu_assert.h"
#include "log.h"

void assertFailed(const char* file, unsigned line, const char* cond, const std::string& msg) {
    logger::error(
        "ASSERTION FAILED\n"
        "FILE: %s\n"
        "LINE: %d\n"
        "CHECK: %s\n"
        "MSG: %s",
        file, line, cond, msg.c_str());
}