#pragma once
#include "debugUIWindow.h"

namespace debug::ui {

class DisassemblerWindow : public Window {
public:
    DisassemblerWindow(const std::string& name) : Window(name), mSelectCol(62, 50, 135) {}

    virtual void draw();

private:
    const ImColor mSelectCol;
    const uint64_t INVALID_SELECTION = 0xFFFFFFFFFFFFFFFF;

    uint64_t mBaseAddr = 0x400100;
    uint64_t mSelectedAddr = INVALID_SELECTION;

    char mAddrInput[19] = { 0 };
};

} // namespace debug::ui
