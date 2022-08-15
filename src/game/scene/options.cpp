#include "options.hpp"

void SceneOptions::render() {
    imgui::frame_start();

    {
        const int TEXT_SCALE = 3;
        const ImVec2 BUTTON_SIZE{700, 60};

        ImGui::TextScalePush(TEXT_SCALE);

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGui::Begin(
            "Options",
            NULL,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings
        );

        ImGui::TextScaledCentered("Options", 10);

        ImGui::Separator();

        if (ImGui::ButtonCentered("back", BUTTON_SIZE))
            EventManager::fire(EventSceneChange{"main_menu"});

        auto shadows = Option<bool>("shadows");
        ImGui::Checkbox("shadows", &shadows);

        auto render_distance = Option("render_distance");
        ImGui::SliderInt("render distance", &render_distance, 4, 32);

        ImGui::TextScalePop();
    }

    imgui::frame_end();
}
