#pragma once
#include <cstdlib>
#include <string>
#include "platform.h"

#ifdef PLATFORM_WINDOWS

#define emu_assert_helper(c, s, m) \
    if (!(c)) { \
        assertFailed(__FILE__, __LINE__, s, m); \
        __debugbreak(); \
        abort(); \
    }

#else

#define emu_assert_helper(c, s, m) \
    if (!(c)) { \
        assertFailed(__FILE__, __LINE__, s, m); \
        __builtin_trap(); \
    }

#endif

#define emu_assert(c, m) \
    emu_assert_helper(c, #c, m)

#define emu_abort(m) \
    emu_assert_helper(false, "abort", m)

#define emu_check(c) \
    emu_assert_helper(c, #c, "")

void assertFailed(const char* file, unsigned line, const char* cond, const std::string& msg);