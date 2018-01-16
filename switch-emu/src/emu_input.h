#pragma once
#include "SDL\include\SDL.h"
// TODO dont use SDL and instead make our our enums

namespace input {

void injectMousePos(float x, float y);
void injectMouseButton(int button, SDL_EventType action);
void injectMouseWheel(float scroll);
void injectKeyEvent(int key, SDL_EventType action);
void injectText(const char* text);

} // namespace input