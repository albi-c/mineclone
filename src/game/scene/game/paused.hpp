#pragma once

#include "window/event.hpp"
#include "game/event.hpp"
#include "game/scene/scene.hpp"
#include "graphics/gui/imgui.hpp"

namespace game {
    class ScenePaused : public Scene {
    public:
        ScenePaused() {}
        ScenePaused(ScenePaused* other) {}

        void init();

        void enable();

        void update(float dt);
        void render();
    
    private:
        inline static ScenePaused* scene;

        FunctionEventQueue<EventKeyPress> key_press_event_queue = FunctionEventQueue<EventKeyPress>(ScenePaused::on_key_press_);
        inline static void on_key_press_(const EventKeyPress& e) {
            scene->on_key_press(e);
        }
        void on_key_press(const EventKeyPress& e);
    };
};

