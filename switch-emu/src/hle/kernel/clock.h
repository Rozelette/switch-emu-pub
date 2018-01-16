#pragma once

namespace kernel::clock {

using ClockCallbackFunc = bool(*)(void);

void init();

void start();
void stop();
void join();

void addCallback(ClockCallbackFunc func);

} // kernel::clock
