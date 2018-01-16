#pragma once
#include <string>

#include "debugUI.h"

namespace debug::ui {

class Window {
public:
    Window(const std::string& name) : mName(name) {}

    const std::string& name() { return mName; }

    void show() { mVisible = true; }
    void hide() { mVisible = false; }
    bool visible() { return mVisible; }
    virtual void draw() = 0;

protected:
    std::string mName;
    bool mVisible;
};

} // namespace debug::ui
