#pragma once
#include <chrono>

#include "debugUIWindow.h"
#include "cpu.h"

namespace debug::ui {

class CoreStateWindow : public Window {
public:
    CoreStateWindow(const std::string& name) : Window(name) {}

    virtual void draw();

private:
    void drawCore(int coreNum);

    std::chrono::time_point<std::chrono::system_clock> mThen, mCurrent;
    std::array<uint64_t, cpu::NUM_CORES> mLastInst;
    std::array<double, cpu::NUM_CORES> mInstPerSec;
    std::array<double, cpu::NUM_CORES> mTotalInstPerSec;
};

} // namespace debug::ui
