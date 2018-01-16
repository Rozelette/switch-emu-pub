#include "platform.h"
#include "platform_thread.h"

#ifdef PLATFORM_WINDOWS

#include <windows.h>

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)

typedef struct tagTHREADNAME_INFO {
    DWORD dwType; // Must be 0x1000.  
    LPCSTR szName; // Pointer to name (in user addr space).  
    DWORD dwThreadID; // Thread ID (-1=caller thread).  
    DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;

#pragma pack(pop)

namespace platform {

void setThreadName(std::thread* thread, const char* threadName) {
    DWORD dwThreadID = ::GetThreadId(static_cast<HANDLE>(thread->native_handle()));

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;

    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
}

void exitThread(int exitCode) {
    ExitThread(exitCode);
}

} // namespace platform

#endif