#pragma once
#include <string>
#include "SDL\include\SDL.h"
#include "graphicsDriver.h"

class Emulator {
private:
    GraphicsDriver* mGraphicsDriver;

public:
    ~Emulator();

    bool initSDL();
    bool initGraphics();

    bool run(const std::string& path);
};