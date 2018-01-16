#pragma once

#if defined(_WIN64)
#define PLATFORM_WINDOWS
#elif __APPLE__
#define PLATFORM_APLLE
#elif __linux__
#define PLATFORM_LINUX
#endif

