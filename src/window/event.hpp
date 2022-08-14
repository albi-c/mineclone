#pragma once

#include "event/event.hpp"

struct EventFramebufferResize : Event {
    int width, height;

    EventFramebufferResize(int width, int height)
        : width(width), height(height) {}
};
struct EventMouseMove : Event {
    double x, y;

    EventMouseMove(double x, double y)
        : x(x), y(y) {}
};
struct EventMouseClick : Event {
    int button, x, y, action, mods;

    EventMouseClick(int button, int x, int y, int action, int mods)
        : button(button), x(x), y(y), action(action), mods(mods) {}
};
struct EventKeyPress : Event {
    int key, scancode, action, mods;

    EventKeyPress(int key, int scancode, int action, int mods)
        : key(key), scancode(scancode), action(action), mods(mods) {}
};
