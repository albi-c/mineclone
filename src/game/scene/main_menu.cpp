#include "main_menu.hpp"

void SceneMainMenu::init() {}

void SceneMainMenu::update() {}
void SceneMainMenu::render() {
    imgui::frame_start();

    {
        const int TEXT_SCALE = 3;
        const ImVec2 BUTTON_SIZE{700, 60};

        ImGui::TextScalePush(TEXT_SCALE);

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGui::Begin(
            "MineClone",
            NULL,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings
        );

        ImGui::Checkbox("ENABLE GUI", &selected);

        if (selected) {
            ImGui::TextScaledCentered("MineClone", 16);

            if (ImGui::ButtonCentered("play", BUTTON_SIZE))
                EventManager::fire(EventSceneChange{"game"});
            if (ImGui::ButtonCentered("options", BUTTON_SIZE))
                EventManager::fire(EventSceneChange{"options"});
            
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.5f, 0.1f, 0.1f));
            if (ImGui::ButtonCentered("exit", BUTTON_SIZE))
                EventManager::fire(EventGameStop{});
            ImGui::PopStyleColor();
        } else {
            ImGui::TextScaledCentered("disabled gui", 5);
        }

        ImGui::End();

        ImGui::TextScalePop();
    }

    imgui::frame_end();
}
