#include "paused.hpp"

namespace game {
    void ScenePaused::render() {
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

            ImGui::TextScaledCentered("Paused", 10);

            ImGui::Separator();

            if (ImGui::ButtonCentered("main menu", BUTTON_SIZE))
                EventManager::fire(EventSceneChange{"main_menu"});

            if (ImGui::ButtonCentered("resume", BUTTON_SIZE))
                EventManager::fire(EventSceneChange{"game:game"});

            ImGui::End();

            ImGui::TextScalePop();
        }

        imgui::frame_end();
    }
};
