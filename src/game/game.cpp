#include "game.hpp"

void Game::init() {
    window = std::make_shared<Window>(new Window());

    EventManager::listen(game_stop_event_queue);
    EventManager::listen(scene_change_event_queue);

    add_scene<SceneMainMenu>("main_menu");
    switch_scene("main_menu");
}

void Game::run() {
    while (window->update()) {
        game_stop_event_queue.process();
        scene_change_event_queue.process();

        window->render_start();

        if (scene) {
            scene->update();
            scene->render();
        }
        
        window->render_end();
    }
}
void Game::stop() {
    window->close();
}

void Game::switch_scene(const std::string& name) {
    scene = scenes[name];
}
template <class T>
void Game::add_scene(const std::string& name) {
    scenes[name] = new T();
    scenes[name]->init();
}
