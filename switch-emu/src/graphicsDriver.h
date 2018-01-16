#pragma once
#include "SDL\include\SDL.h"
#include <thread>
#include <atomic>

#include "glbinding\gl\gl.h"

class GraphicsDriver {
private:
    std::thread mGraphicsThread;
    SDL_Window* mWindow = nullptr;
    SDL_GLContext mContext;
    SDL_GLContext mThreadContext;
    std::atomic<bool> mRunning;

    gl::GLfloat mBackground[3];
    gl::GLuint* displayBuffer;
public:
    GraphicsDriver();
    virtual ~GraphicsDriver();

    bool init();
    void run();
    void stop();

    void setBuffer(uint64_t addr);
};
