#include "game/scene/game/paused.hpp"

namespace game {
    void ScenePaused::init() {
        scene = this;

        EventManager::listen(key_press_event_queue);
    }

    void ScenePaused::enable() {
        key_press_event_queue.clear();
    }

    void ScenePaused::update(float dt) {
        key_press_event_queue.process();
    }
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

            ImGui::TextScalePop();
        }

        imgui::frame_end();
    }

    void ScenePaused::on_key_press(const EventKeyPress& e) {
        if (e.key == GLFW_KEY_ESCAPE && e.action == GLFW_PRESS)
            EventManager::fire(EventSceneChange{"game:game"});
    }
};
