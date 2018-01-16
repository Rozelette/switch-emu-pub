#include <fstream>

#include "emu.h"
#include "cpu.h"
#include "mem.h"
#include "hle\kernel\loader\loader.h"
#include "log.h"
#include "hle\kernel\kernel.h"
#include "emu_input.h"
#include "debug\debug.h"

Emulator::~Emulator() {
    if (mGraphicsDriver) {
        delete mGraphicsDriver;
    }
    SDL_Quit();
}

bool Emulator::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        logger::error("Failed to init SDL: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool Emulator::initGraphics() {
    mGraphicsDriver = new GraphicsDriver();

    if (!mGraphicsDriver->init()) {
        logger::error("Failed to init graphics driver: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool Emulator::run(const std::string& path) {
    cpu::init();

    if (!mem::init(0x100000000ull)) {
        logger::error("Coudn't init memory");
        return false;
    }

    if (!kernel::init()) {
        logger::error("Coudn't init kernel");
        return false;
    }
    kernel::setGraphicsDriver(mGraphicsDriver);
    kernel::setGameCartPath(path);

    if (!kernel::launchGameCart()) {
        logger::error("Coudn't launch game");
        return false;
    }

    debug::init();

    cpu::start();

    bool shouldStop = false;

    while (!shouldStop) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            {
                switch (event.type) {
                case SDL_MOUSEWHEEL:
                {
                    input::injectMouseWheel(static_cast<float>(event.wheel.y));
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    input::injectMousePos(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    input::injectMouseButton(event.button.button, static_cast<SDL_EventType>(event.type));
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    input::injectMouseButton(event.button.button, static_cast<SDL_EventType>(event.type));
                    break;
                }
                case SDL_TEXTINPUT:
                {
                    input::injectText(event.text.text);
                    break;
                }
                case SDL_KEYDOWN:
                {
                    input::injectKeyEvent(event.key.keysym.sym & ~SDLK_SCANCODE_MASK, static_cast<SDL_EventType>(event.type));
                    break;
                }
                case SDL_KEYUP:
                {
                    input::injectKeyEvent(event.key.keysym.sym & ~SDLK_SCANCODE_MASK, static_cast<SDL_EventType>(event.type));
                    break;
                }
                }
            }
            if (event.type == SDL_QUIT) {
                shouldStop = true;
            }
        }
    }

    kernel::exit();

    cpu::halt();
    cpu::join();

    if (mGraphicsDriver) {
        mGraphicsDriver->stop();
    }

    return true;
}