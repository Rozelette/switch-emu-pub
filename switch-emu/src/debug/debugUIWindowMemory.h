#pragma once
#include "debugUIWindow.h"

namespace debug::ui {

class MemoryWindow : public Window {
public:
    MemoryWindow(const std::string& name) : Window(name) {}

    virtual void draw();

private:
    uint64_t mBaseAddr = 0x400100;
    char mAddrInput[19] = { 0 };
};

} // namespace debug::ui
