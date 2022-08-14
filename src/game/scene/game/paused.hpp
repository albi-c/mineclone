#pragma once

#include "game/event.hpp"
#include "game/scene/scene.hpp"
#include "graphics/gui/imgui.hpp"

namespace game {
    class ScenePaused : public Scene {
    public:
        ScenePaused() {}
        ScenePaused(ScenePaused* other) {}

        void render();
    };
};

