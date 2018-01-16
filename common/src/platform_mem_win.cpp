#include "platform.h"
#include "platform_mem.h"
#include "unreachable.h"

#ifdef PLATFORM_WINDOWS

#include <windows.h>

namespace platform {

DWORD flagsToWindowsEnum(PageFlags flags) {
    switch (flags) {
    case platform::PageFlags::NONE:
        return PAGE_NOACCESS;
    case platform::PageFlags::READ_ONLY:
        return PAGE_READONLY;
    case platform::PageFlags::READ_WRITE:
        return PAGE_READWRITE;
    case platform::PageFlags::READ_EXECUTE:
        return PAGE_EXECUTE_READ;
    case platform::PageFlags::ALL:
        return PAGE_EXECUTE_READWRITE;
    default:
        UNREACHABLE();
    }
}

void* allocate(uint64_t size, PageFlags flags) {
    auto page = VirtualAlloc(NULL, size, MEM_COMMIT, flagsToWindowsEnum(flags));
    if (!page) {
        return nullptr;
    } else {
        return reinterpret_cast<void*>(page);
    }
}

void free(void* addr) {
    // TODO error checking
    VirtualFree(reinterpret_cast<LPVOID>(addr), 0, MEM_RELEASE);
}

} // namespace platform

#endif