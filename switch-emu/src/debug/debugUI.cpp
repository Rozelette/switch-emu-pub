#include <vector>

#include "debugUI.h"
#include "debugUIWindowCoreState.h"
#include "debugUIWindowMemory.h"
#include "debugUIWindowDisassembler.h"

// TODO don't use SDL
#include "SDL\include\SDL.h"

namespace debug::ui {

std::vector<Window*> gWindows;

static struct UIState {
    double time = 0.0f;
    bool mousePressed[3] = { false, false, false };
    float mouseWheel = 0.0f;
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    gl::GLuint fontTexture = 0;
    int shaderHandle = 0, vertHandle = 0, fragHandle = 0;
    int attribLocationTex = 0, attribLocationProjMtx = 0;
    int attribLocationPosition = 0, attribLocationUV = 0, attribLocationColor = 0;
    unsigned int vboHandle = 0, vaoHandle = 0, elementsHandle = 0;
} sState;


static const gl::GLchar* sVertexShader =
    "#version 330\n"
    "uniform mat4 ProjMtx;\n"
    "in vec2 Position;\n"
    "in vec2 UV;\n"
    "in vec4 Color;\n"
    "out vec2 Frag_UV;\n"
    "out vec4 Frag_Color;\n"
    "void main()\n"
    "{\n"
    "    Frag_UV = UV;\n"
    "    Frag_Color = Color;\n"
    "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
    "}\n";

static const gl::GLchar* sFragmentShader =
    "#version 330\n"
    "uniform sampler2D Texture;\n"
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "out vec4 Out_Color;\n"
    "void main()\n"
    "{\n"
    "    Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
    "}\n";

void createFontTexture() {
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    gl::GLint last_texture;
    gl::glGetIntegerv(gl::GL_TEXTURE_BINDING_2D, &last_texture);
    gl::glGenTextures(1, &sState.fontTexture);
    gl::glBindTexture(gl::GL_TEXTURE_2D, sState.fontTexture);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);
    gl::glPixelStorei(gl::GL_UNPACK_ROW_LENGTH, 0);
    gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGBA, width, height, 0, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, pixels);

    io.Fonts->TexID = (void *)(intptr_t)sState.fontTexture;

    gl::glBindTexture(gl::GL_TEXTURE_2D, last_texture);
}

void initFontTexture() {
    // Backup GL state
    gl::GLint last_texture, last_array_buffer, last_vertex_array;

    gl::glGetIntegerv(gl::GL_TEXTURE_BINDING_2D, &last_texture);
    gl::glGetIntegerv(gl::GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    gl::glGetIntegerv(gl::GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    sState.shaderHandle = gl::glCreateProgram();
    sState.vertHandle = gl::glCreateShader(gl::GL_VERTEX_SHADER);
    sState.fragHandle = gl::glCreateShader(gl::GL_FRAGMENT_SHADER);
    gl::glShaderSource(sState.vertHandle, 1, &sVertexShader, 0);
    gl::glShaderSource(sState.fragHandle, 1, &sFragmentShader, 0);
    gl::glCompileShader(sState.vertHandle);
    gl::glCompileShader(sState.fragHandle);
    gl::glAttachShader(sState.shaderHandle, sState.vertHandle);
    gl::glAttachShader(sState.shaderHandle, sState.fragHandle);
    gl::glLinkProgram(sState.shaderHandle);

    sState.attribLocationTex = gl::glGetUniformLocation(sState.shaderHandle, "Texture");
    sState.attribLocationProjMtx = gl::glGetUniformLocation(sState.shaderHandle, "ProjMtx");
    sState.attribLocationPosition = gl::glGetAttribLocation(sState.shaderHandle, "Position");
    sState.attribLocationUV = gl::glGetAttribLocation(sState.shaderHandle, "UV");
    sState.attribLocationColor = gl::glGetAttribLocation(sState.shaderHandle, "Color");

    gl::glGenBuffers(1, &sState.vboHandle);
    gl::glGenBuffers(1, &sState.elementsHandle);

    gl::glGenVertexArrays(1, &sState.vaoHandle);
    gl::glBindVertexArray(sState.vaoHandle);
    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, sState.vboHandle);
    gl::glEnableVertexAttribArray(sState.attribLocationPosition);
    gl::glEnableVertexAttribArray(sState.attribLocationUV);
    gl::glEnableVertexAttribArray(sState.attribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    gl::glVertexAttribPointer(sState.attribLocationPosition, 2, gl::GL_FLOAT, gl::GL_FALSE, sizeof(ImDrawVert), (gl::GLvoid*)OFFSETOF(ImDrawVert, pos));
    gl::glVertexAttribPointer(sState.attribLocationUV, 2, gl::GL_FLOAT, gl::GL_FALSE, sizeof(ImDrawVert), (gl::GLvoid*)OFFSETOF(ImDrawVert, uv));
    gl::glVertexAttribPointer(sState.attribLocationColor, 4, gl::GL_UNSIGNED_BYTE, gl::GL_TRUE, sizeof(ImDrawVert), (gl::GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

    createFontTexture();

    // Restore modified GL state
    gl::glBindTexture(gl::GL_TEXTURE_2D, last_texture);
    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, last_array_buffer);
    gl::glBindVertexArray(last_vertex_array);
}

void uiDraw(ImDrawData* draw_data) {
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    gl::GLint last_program; gl::glGetIntegerv(gl::GL_CURRENT_PROGRAM, &last_program);
    gl::GLint last_texture; gl::glGetIntegerv(gl::GL_TEXTURE_BINDING_2D, &last_texture);
    gl::GLint last_active_texture; gl::glGetIntegerv(gl::GL_ACTIVE_TEXTURE, &last_active_texture);
    gl::GLint last_array_buffer; gl::glGetIntegerv(gl::GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    gl::GLint last_element_array_buffer; gl::glGetIntegerv(gl::GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    gl::GLint last_vertex_array; gl::glGetIntegerv(gl::GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    gl::GLint last_blend_src; gl::glGetIntegerv(gl::GL_BLEND_SRC, &last_blend_src);
    gl::GLint last_blend_dst; gl::glGetIntegerv(gl::GL_BLEND_DST, &last_blend_dst);
    gl::GLint last_blend_equation_rgb; gl::glGetIntegerv(gl::GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    gl::GLint last_blend_equation_alpha; gl::glGetIntegerv(gl::GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    gl::GLint last_viewport[4]; gl::glGetIntegerv(gl::GL_VIEWPORT, last_viewport);
    gl::GLint last_scissor_box[4]; gl::glGetIntegerv(gl::GL_SCISSOR_BOX, last_scissor_box);
    gl::GLboolean last_enable_blend = gl::glIsEnabled(gl::GL_BLEND);
    gl::GLboolean last_enable_cull_face = gl::glIsEnabled(gl::GL_CULL_FACE);
    gl::GLboolean last_enable_depth_test = gl::glIsEnabled(gl::GL_DEPTH_TEST);
    gl::GLboolean last_enable_scissor_test = gl::glIsEnabled(gl::GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    gl::glEnable(gl::GL_BLEND);
    gl::glBlendEquation(gl::GL_FUNC_ADD);
    gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);
    gl::glDisable(gl::GL_CULL_FACE);
    gl::glDisable(gl::GL_DEPTH_TEST);
    gl::glEnable(gl::GL_SCISSOR_TEST);
    gl::glActiveTexture(gl::GL_TEXTURE0);

    // Setup orthographic projection matrix
    gl::glViewport(0, 0, (gl::GLsizei)fb_width, (gl::GLsizei)fb_height);
    const float ortho_projection[4][4] =
    {
        { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        { -1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    gl::glUseProgram(sState.shaderHandle);
    gl::glUniform1i(sState.attribLocationTex, 0);
    gl::glUniformMatrix4fv(sState.attribLocationProjMtx, 1, gl::GL_FALSE, &ortho_projection[0][0]);
    gl::glBindVertexArray(sState.vaoHandle);

    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        gl::glBindBuffer(gl::GL_ARRAY_BUFFER, sState.vboHandle);
        gl::glBufferData(gl::GL_ARRAY_BUFFER, (gl::GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (gl::GLvoid*)cmd_list->VtxBuffer.Data, gl::GL_STREAM_DRAW);

        gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, sState.elementsHandle);
        gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER, (gl::GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (gl::GLvoid*)cmd_list->IdxBuffer.Data, gl::GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            } else {
                gl::glBindTexture(gl::GL_TEXTURE_2D, (gl::GLuint)(intptr_t)pcmd->TextureId);
                gl::glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                gl::glDrawElements(gl::GL_TRIANGLES, (gl::GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? gl::GL_UNSIGNED_SHORT : gl::GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore modified GL state
    gl::glUseProgram(last_program);
    gl::glActiveTexture((gl::GLenum)last_active_texture);
    gl::glBindTexture(gl::GL_TEXTURE_2D, last_texture);
    gl::glBindVertexArray(last_vertex_array);
    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, last_array_buffer);
    gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    gl::glBlendEquationSeparate((gl::GLenum)last_blend_equation_rgb, (gl::GLenum)last_blend_equation_alpha);
    gl::glBlendFunc((gl::GLenum)last_blend_src, (gl::GLenum)last_blend_dst);
    if (last_enable_blend) gl::glEnable(gl::GL_BLEND); else gl::glDisable(gl::GL_BLEND);
    if (last_enable_cull_face) gl::glEnable(gl::GL_CULL_FACE); else gl::glDisable(gl::GL_CULL_FACE);
    if (last_enable_depth_test) gl::glEnable(gl::GL_DEPTH_TEST); else gl::glDisable(gl::GL_DEPTH_TEST);
    if (last_enable_scissor_test) gl::glEnable(gl::GL_SCISSOR_TEST); else gl::glDisable(gl::GL_SCISSOR_TEST);
    gl::glViewport(last_viewport[0], last_viewport[1], (gl::GLsizei)last_viewport[2], (gl::GLsizei)last_viewport[3]);
    gl::glScissor(last_scissor_box[0], last_scissor_box[1], (gl::GLsizei)last_scissor_box[2], (gl::GLsizei)last_scissor_box[3]);
}

bool init() {
    // TODO use emu enums not SDL
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDLK_a;
    io.KeyMap[ImGuiKey_C] = SDLK_c;
    io.KeyMap[ImGuiKey_V] = SDLK_v;
    io.KeyMap[ImGuiKey_X] = SDLK_x;
    io.KeyMap[ImGuiKey_Y] = SDLK_y;
    io.KeyMap[ImGuiKey_Z] = SDLK_z;

    io.RenderDrawListsFn = uiDraw;

    io.GetClipboardTextFn = [](void *) -> const char* {
        return SDL_GetClipboardText();
    };

    io.SetClipboardTextFn = [](void *, const char *text) {
        SDL_SetClipboardText(text);
    };
    io.ClipboardUserData = NULL;

    gWindows.push_back(new CoreStateWindow("CoreState"));
    gWindows.push_back(new DisassemblerWindow("Disassembly"));
    gWindows.push_back(new MemoryWindow("Memory"));

    return true;
}

void draw() {
    if (!sState.fontTexture) {
        initFontTexture();
    }

    ImGuiIO& io = ImGui::GetIO();

    int w, h;
    int display_w, display_h;
    //SDL_GetWindowSize(window, &w, &h);
    //SDL_GL_GetDrawableSize(window, &display_w, &display_h);
    // TODO update per frame;
    w = 1280;
    h = 720;
    display_w = 1280;
    display_h = 720;
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from SDL_PollEvent())
    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    //if (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    //else
    //    io.MousePos = ImVec2(-1, -1);

    io.MouseDown[0] = sState.mousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    io.MouseDown[1] = sState.mousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = sState.mousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    sState.mousePressed[0] = sState.mousePressed[1] = sState.mousePressed[2] = false;

    io.MouseWheel = sState.mouseWheel;
    sState.mouseWheel = 0.0f;

    ImGui::NewFrame();

    for (auto& window : gWindows) {
        window->draw();
    }

    gl::glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    ImGui::Render();
}

void handleMouseMove(float x, float y) {
    sState.mouseX = x;
    sState.mouseY = y;
}

void handleMouseButton(int button, SDL_EventType action) {
    switch (button) {
    case SDL_BUTTON_LEFT:
        if (action == SDL_MOUSEBUTTONDOWN) {
            sState.mousePressed[0] = true;
        } else if (action == SDL_MOUSEBUTTONUP) {
            sState.mousePressed[0] = false;
        }
        break;
    case SDL_BUTTON_RIGHT:
        if (action == SDL_MOUSEBUTTONDOWN) {
            sState.mousePressed[1] = true;
        } else if (action == SDL_MOUSEBUTTONUP) {
            sState.mousePressed[1] = false;
        }
        break;
    case SDL_BUTTON_MIDDLE:
        if (action == SDL_MOUSEBUTTONDOWN) {
            sState.mousePressed[2] = true;
        } else if (action == SDL_MOUSEBUTTONUP) {
            sState.mousePressed[2] = false;
        }
        break;
    default:
        break;
    }
}

void handleMouseScroll(float scrollx, float scrolly) {
    sState.mouseWheel += scrollx;
}

void handleKeyEvent(int button, SDL_EventType action) {
    ImGuiIO& io = ImGui::GetIO();

    io.KeysDown[button] = (action == SDL_KEYDOWN);
    io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
    io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
    io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
    io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
}

void handleTextEvent(const char* text) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharactersUTF8(text);
}

} // namespace debugger::ui