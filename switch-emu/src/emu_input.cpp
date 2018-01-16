#include "emu_input.h"
#include "debug\debugUI.h"

namespace input {

void injectMousePos(float x, float y) {
    debug::ui::handleMouseMove(x, y);
}
void injectMouseButton(int button, SDL_EventType action) {
    debug::ui::handleMouseButton(button, action);
}

void injectMouseWheel(float scroll) {
    debug::ui::handleMouseScroll(scroll, 0.0f);
}

void injectKeyEvent(int key, SDL_EventType action) {
    debug::ui::handleKeyEvent(key, action);
}

void injectText(const char* text) {
    debug::ui::handleTextEvent(text);
}

} // namespace input