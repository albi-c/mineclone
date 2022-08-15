#pragma once

#include <string>

#include "event/event.hpp"

struct EventGameStop : Event {};
struct EventSceneChange : Event {
    std::string scene;

    EventSceneChange(const std::string& scene)
        : scene(scene) {}
};

struct EventOptionChange : Event {
    std::string name;
    int value;

    EventOptionChange(std::string name, int value)
        : name(name), value(value) {}
};
