#pragma once
#include <stdint.h>

namespace platform {

enum class PageFlags {
    NONE,
    READ_ONLY,
    READ_WRITE,
    READ_EXECUTE,
    ALL
};

void* allocate(uint64_t size, PageFlags flags);
void free(void* addr);

}