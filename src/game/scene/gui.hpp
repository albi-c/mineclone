#pragma once

#include "scene.hpp"
#include "game/event.hpp"
#include "graphics/gui/imgui.hpp"

class SceneGui : public Scene {
public:
    virtual void init();

    virtual void update();
    virtual void render();
};
