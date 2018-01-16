#pragma once
#include <thread>

namespace platform {

void setThreadName(std::thread* thread, const char* threadName);

void exitThread(int exitCode);

} // namespace platform