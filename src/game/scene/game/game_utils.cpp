#include "game.hpp"

namespace game {
    void SceneGameHandlers::listen() {
        EventManager::listen(mouse_move_event_queue);
        EventManager::listen(mouse_click_event_queue);
    }
    void SceneGameHandlers::process() {
        mouse_move_event_queue.process();
        mouse_click_event_queue.process();
    }

    void SceneGameHandlers::event_mouse_move_handler(const EventMouseMove& e) {
        game->on_mouse_move(e);
    }
    void SceneGameHandlers::event_mouse_click_handler(const EventMouseClick& e) {
        game->on_mouse_click(e);
    }
};
