#include "game.hpp"

namespace game {
    void SceneGameHandlers::listen() {
        EventManager::listen(window_resize_event_handler);
        EventManager::listen(mouse_move_event_queue);
        EventManager::listen(mouse_click_event_queue);
        EventManager::listen(key_press_event_queue);
    }
    void SceneGameHandlers::process() {
        window_resize_event_handler.process();
        mouse_move_event_queue.process();
        mouse_click_event_queue.process();
        key_press_event_queue.process();
    }
    void SceneGameHandlers::clear() {
        window_resize_event_handler.clear();
        mouse_move_event_queue.clear();
        mouse_click_event_queue.clear();
        key_press_event_queue.clear();
    }

    void SceneGameHandlers::event_window_resize_handler(const EventFramebufferResize& e) {
        game->on_window_resize(e);
    }
    void SceneGameHandlers::event_mouse_move_handler(const EventMouseMove& e) {
        game->on_mouse_move(e);
    }
    void SceneGameHandlers::event_mouse_click_handler(const EventMouseClick& e) {
        game->on_mouse_click(e);
    }
    void SceneGameHandlers::event_key_press_handler(const EventKeyPress& e) {
        game->on_key_press(e);
    }
};
