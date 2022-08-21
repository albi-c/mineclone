#pragma once

#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <utility>
#include <thread>
#include <utility>
#include <mutex>

#include "window/window.hpp"
#include "graphics/textures/texture.hpp"
#include "graphics/textures/texture_array.hpp"
#include "util/ray.hpp"
#include "event/event.hpp"
#include "util/string.hpp"
#include "resources/loader.hpp"
#include "world/world.hpp"

#include "event.hpp"

#include "task/task.hpp"

#include "scene/scene.hpp"
#include "scene/main_menu.hpp"
#include "scene/options.hpp"
#include "scene/game/game.hpp"
#include "scene/game/paused.hpp"

class Game {
public:
    static void init();

    static void run();
    static void stop();

    static void switch_scene(const std::string& name);
    template <class T>
    static void add_scene(const std::string& name);

private:
    static inline Window* window;

    static inline std::map<std::string, Scene*> scenes;
    static inline Scene* scene;

    static inline bool running;

    static std::thread* create_worker_thread();

    static inline FunctionEventQueue<EventGameStop> game_stop_event_queue = FunctionEventQueue<EventGameStop>([](const EventGameStop& e) {
        Game::stop();
    });
    static inline FunctionEventQueue<EventSceneChange> scene_change_event_queue = FunctionEventQueue<EventSceneChange>([](const EventSceneChange& e) {
        Game::switch_scene(e.scene);
    });
};
