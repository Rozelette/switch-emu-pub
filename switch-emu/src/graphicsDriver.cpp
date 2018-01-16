#include <thread>
#include <iostream>

#include "graphicsDriver.h"
#include "glbinding\gl\gl.h"
#include "glbinding\Binding.h"
#include "glbinding\Meta.h"
#include "debug\debugUI.h"
#include "log.h"
#include "platform_thread.h"
#include "hle\kernel\kernel.h"
#include "mem.h"
#include "hle\nn\vi\Display.h"

extern nn::vi::emu::DisplayManager gDisplayManager;

using namespace gl;

const bool gDrawDebugWindow = true;

static const gl::GLchar* gVertexShader =
"#version 330\n"
"in vec2 position;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(position.xy,0,1);\n"
"}\n";

static const gl::GLchar* gFragmentShader =
"#version 330\n"
"in vec2 Texcoord;\n"
"layout(origin_upper_left) in vec4 gl_FragCoord;\n"
"\n"
"uniform sampler2D tex;\n"
"\n"
"out vec4 outColor;\n"
"\n"
"void main()\n"
"{\n"
"    int x = int(gl_FragCoord.x);\n"
"    int y = int(gl_FragCoord.y);\n"
"    int GOBaddress = ((y & 127) / 16) + (x / 16 * 8) + ((y / 16 / 8) * (1280 / 16 * 8));\n"
"    int pixeladdress = GOBaddress * 16 * 16 + ((y % 16) / 8) * 128 + ((x % 16) / 8) * 64 + ((y % 8) / 2) * 16 + ((x % 8) / 4) * 8 + (y % 2) * 4 + (x % 4);\n"
"    vec2 pixelLoc = vec2(fract((float(pixeladdress) + 0.5) / 1280.0), fract((float(pixeladdress / 1280) + 0.5) / 768.0));\n"
"    outColor = texture(tex, pixelLoc.xy);\n"
"}\n";

GraphicsDriver::GraphicsDriver() {
    mBackground[0] = 230.0f / 255.0f;
    mBackground[1] = 0.0f;
    mBackground[2] = 18.0f / 255.0f;

    mRunning.store(false);
}

GraphicsDriver::~GraphicsDriver() {
    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

bool GraphicsDriver::init() {
    if (SDL_GL_LoadLibrary(NULL) != 0) {
        logger::error("Could not load opengl library: %s", SDL_GetError());
        return false;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    mWindow = SDL_CreateWindow("switch-emu",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1280,
        720,
        SDL_WINDOW_OPENGL);

    if (!mWindow) {
        logger::error("Could not create window: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    mContext = SDL_GL_CreateContext(mWindow);

    if (!mContext) {
        logger::error("Could not create context: %s", SDL_GetError());
        return false;
    }

    mThreadContext = SDL_GL_CreateContext(mWindow);

    if (!mThreadContext) {
        logger::error("Could not create context: %s", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(mWindow, mContext);

    mGraphicsThread = std::thread([this]() {
        SDL_GL_MakeCurrent(mWindow, mThreadContext);
        glbinding::Binding::initialize();

#ifndef _DEBUG
        // setCallbackMaskExcept cause an access violation at runtime in debug for some reason
        // TODO figure this out
        glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, { "glGetError" });
        glbinding::setAfterCallback([](const glbinding::FunctionCall &call) {
            auto error = glGetError();
            if (error != GL_NO_ERROR) {
                std::string msg = "";
                msg += glbinding::Meta::getString(error) + " at ";
                msg += call.function->name();
                msg += "(";
                for (auto i = 0u; i < call.parameters.size(); i++) {
                    msg += call.parameters[i]->asString();
                    if (i != call.parameters.size() - 1) {
                        msg += ", ";
                    }
                }
                msg += ")";
                if (call.returnValue) {
                    msg += " -> ";
                    msg += call.returnValue->asString();
                }

                logger::error("OpenGL error: %s", msg.c_str());
            }
        });
#endif // !_DEBUG

        mRunning.store(true);

        run();
    });

    platform::setThreadName(&mGraphicsThread, "Graphics Thread");

    displayBuffer = new GLuint[0x3c0000 / 4];
    memset(displayBuffer, 0, 0x3c0000 / 4 * sizeof(GLuint));

    return true;
}

void GraphicsDriver::run() {
    SDL_GL_MakeCurrent(mWindow, mThreadContext);

    // TODO error checking
    GLint status;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &gVertexShader, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status != GLint(GL_TRUE)) {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);

        logger::error("Could not compile vertex shader. Log:\n%s", errorLog.data());

        glDeleteShader(vertexShader);
        return;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &gFragmentShader, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status != GLint(GL_TRUE)) {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);

        logger::error("Could not compile fragment shader. Log:\n%s", errorLog.data());

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    float vertices[] = {
        -1.0f,  1.0f, // tl
         1.0f,  1.0f, // tr
         1.0f, -1.0f, // br
        -1.0f, -1.0f, // bl
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLuint elements[] = {
        0, 1, 2,
        0, 2, 3
    };

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    GLuint displayTex;
    glGenTextures(1, &displayTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, displayTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 768 /* ALIGN_UP(720, 64) */, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, displayBuffer);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    while (mRunning.load()) {
        glClearColor(mBackground[0], mBackground[1], mBackground[2], 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        gDisplayManager.draw();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, displayTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1280, 768, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, (GLvoid*)displayBuffer);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        if (gDrawDebugWindow) {
            debug::ui::draw();
        }

        SDL_GL_SwapWindow(mWindow);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    // TODO cleanup
    delete[] displayBuffer;
}

void GraphicsDriver::stop() {
    mRunning.store(false);
    if (mGraphicsThread.joinable()) {
        mGraphicsThread.join();
    }
}

void GraphicsDriver::setBuffer(uint64_t addr) {
    mem::readRegion(addr, reinterpret_cast<uint8_t*>(displayBuffer), 0x3c0000); // TODO this copies into the buffer. Could we avoid the copy?
}