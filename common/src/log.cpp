#include <stdio.h>
#include <cstdarg>
#include <mutex>

namespace logger {

std::mutex gLogMutex;

void error(std::string s, ...) {
    va_list args;
    va_start(args, s);

    std::unique_lock<std::mutex> lock(gLogMutex);
    printf("[error] ");
    vprintf(s.c_str(), args);
    printf("\n");

    va_end(args);
}

void warn(std::string s, ...) {
    va_list args;
    va_start(args, s);

    std::unique_lock<std::mutex> lock(gLogMutex);
    printf("[warn] ");
    vprintf(s.c_str(), args);
    printf("\n");

    va_end(args);
}

void info(std::string s, ...) {
    va_list args;
    va_start(args, s);

    std::unique_lock<std::mutex> lock(gLogMutex);
    printf("[info] ");
    vprintf(s.c_str(), args);
    printf("\n");

    va_end(args);
}

}