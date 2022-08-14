#pragma once

#include <string>

#include "event/event.hpp"

struct EventGameStop : Event {};
struct EventSceneChange : Event {
    std::string scene;

    EventSceneChange(const std::string& scene)
        : scene(scene) {}
};
