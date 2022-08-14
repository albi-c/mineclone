#include "imgui.hpp"

namespace imgui {
    void init(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        for (int i = 0; i < IMGUI_NUM_FONT_SIZES; i++) {
            fonts[i] = add_default_font(i * 13.0f);
        }
    }
    void destroy() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void frame_start() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    void frame_end() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    ImFont* add_default_font(float size) {
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig config;
        config.SizePixels = size;
        config.OversampleH = config.OversampleV = 1;
        config.PixelSnapH = true;
        ImFont* font = io.Fonts->AddFontDefault(&config);
        return font;
    }
};

namespace ImGui {
    void TextCentered(const std::string& text) {
        float windowWidth = ImGui::GetWindowSize().x;
        float textWidth =   ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text("%s", text.c_str());
    }
    void TextScaled(const std::string& text, int scale) {
        TextScalePush(scale);
        ImGui::Text("%s", text.c_str());
        TextScalePop();
    }
    void TextScaledCentered(const std::string& text, int scale) {
        TextScalePush(scale);
        TextCentered(text);
        TextScalePop();
    }

    bool ButtonCentered(const std::string& text, const ImVec2& size) {
        float windowWidth = ImGui::GetWindowSize().x;
        float textWidth =   ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - size.x) * 0.5f);
        return ImGui::Button(text.c_str(), size);
    }

    void TextScalePush(int scale) {
        ImGui::PushFont(imgui::fonts[scale - 1]);
    }
    void TextScalePop() {
        ImGui::PopFont();
    }
};
