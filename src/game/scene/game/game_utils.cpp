#include "game.hpp"

namespace game {
    void SceneGameHandlers::listen() {
        EventManager::listen(window_resize_event_handler);
        EventManager::listen(mouse_move_event_queue);
        EventManager::listen(mouse_click_event_queue);
        EventManager::listen(key_press_event_queue);
        EventManager::listen(option_change_event_queue);
        EventManager::listen(chunk_load_event_queue);
    }
    void SceneGameHandlers::process() {
        window_resize_event_handler.process();
        mouse_move_event_queue.process();
        mouse_click_event_queue.process();
        key_press_event_queue.process();
        // option_change_event_queue.process();
        // chunk_load_event_queue.process();
    }
    void SceneGameHandlers::clear() {
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
    void SceneGameHandlers::event_option_change_handler(const EventOptionChange& e) {
        game->on_option_change(e);
    }
    void SceneGameHandlers::event_chunk_load_handler(const EventChunkLoad& e) {
        game->on_chunk_load(e);
    }
    void SceneGameHandlers::event_chunk_unload_handler(const EventChunkUnload& e) {
        game->on_chunk_unload(e);
    }
};
