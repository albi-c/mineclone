#pragma once

#include "scene.hpp"
#include "game/event.hpp"
#include "graphics/gui/imgui.hpp"

class SceneMainMenu : public Scene {
public:
    SceneMainMenu() {}
    SceneMainMenu(SceneMainMenu* other) {}

    void init();

    void update();
    void render();

private:
    bool selected = false;
};
