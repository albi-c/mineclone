#pragma once

#include "scene.hpp"
#include "game/event.hpp"
#include "game/options.hpp"
#include "graphics/gui/imgui.hpp"

class SceneOptions : public Scene {
public:
    SceneOptions() {}
    SceneOptions(SceneOptions* other) {}

    void render();
};
