#pragma once
#include "imgui\imgui.h"
#include "glbinding\gl\gl.h"

// TODO don't use SDL
#include "SDL\include\SDL.h"

namespace debug::ui {

bool init();

void handleMouseMove(float x, float y);
void handleMouseButton(int button, SDL_EventType action);
void handleMouseScroll(float scrollx, float scrolly);
void handleKeyEvent(int button, SDL_EventType action);
void handleTextEvent(const char* text);

void draw();

} // namespace debug::ui