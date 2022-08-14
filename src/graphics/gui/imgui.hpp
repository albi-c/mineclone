#pragma once

#include "lib/opengl.hpp"

#include "lib/imgui/imgui.h"
#include "lib/imgui/backends/imgui_impl_glfw.h"
#include "lib/imgui/backends/imgui_impl_opengl3.h"

#define IMGUI_NUM_FONT_SIZES 16

namespace imgui {
    void init(GLFWwindow* window);
    void destroy();

    void frame_start();
    void frame_end();

    ImFont* add_default_font(float size);

    inline ImFont* fonts[IMGUI_NUM_FONT_SIZES];
};

namespace ImGui {
    void TextCentered(const std::string& text);
    void TextScaled(const std::string& text, int scale);
    void TextScaledCentered(const std::string& text, int scale);

    bool ButtonCentered(const std::string& text, const ImVec2& size);

    void TextScalePush(int scale);
    void TextScalePop();
};
