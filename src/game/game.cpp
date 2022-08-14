#include "game.hpp"

void Game::init() {
    window = std::make_shared<Window>(new Window());

    EventManager::listen(game_stop_event_queue);
    EventManager::listen(scene_change_event_queue);

    add_scene<SceneMainMenu>("main_menu");
    switch_scene("main_menu");

    add_scene<SceneOptions>("options");

    add_scene<game::SceneGame>("game:game");
    add_scene<game::ScenePaused>("game:paused");
}

void Game::run() {
    double dt;
    while (window->update(dt)) {
        game_stop_event_queue.process();
        scene_change_event_queue.process();

        window->render_start();

        if (scene) {
            scene->update(dt);
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
