#include "options.hpp"

void SceneOptions::init() {}

void SceneOptions::update() {}
void SceneOptions::render() {
    imgui::frame_start();

    {
        const int TEXT_SCALE = 4;
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

        if (ImGui::ButtonCentered("back", BUTTON_SIZE))
            EventManager::fire(EventSceneChange{"main_menu"});

        ImGui::Checkbox("shadows", &Options::shadows);

        ImGui::End();

        ImGui::TextScalePop();
    }

    imgui::frame_end();
}
