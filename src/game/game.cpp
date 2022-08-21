#include "game.hpp"

#define WORKER_THREADS 8

void Game::init() {
    window = new Window();

    TaskScheduler::init(2);

    EventManager::listen(game_stop_event_queue);
    EventManager::listen(scene_change_event_queue);

    add_scene<SceneMainMenu>("main_menu");
    switch_scene("main_menu");

    add_scene<SceneOptions>("options");

    add_scene<game::SceneGame>("game:game");
    add_scene<game::ScenePaused>("game:paused");
}

void Game::run() {
    static double dt;
    static bool pressed_keys[GLFW_KEY_LAST + 1];
    running = true;

    std::thread update_thread([&]() {
        while ((running = window->update(dt))) {
            if (scene) {
                scene->update(dt);
                window->pressed_keys(pressed_keys);
                scene->update_keyboard(dt, pressed_keys);
            }

            game_stop_event_queue.process();
            scene_change_event_queue.process();
        }
    });

    std::thread* worker_threads[WORKER_THREADS];
    for (int i = 0; i < WORKER_THREADS; i++) {
        worker_threads[i] = create_worker_thread();
    }

    while (running) {
        window->render_start();

        if (scene) {
            scene->render();
        }
        
        window->render_end();
    }

    update_thread.join();

    for (int i = 0; i < WORKER_THREADS; i++) {
        worker_threads[i]->join();
        delete worker_threads[i];
    }
}
void Game::stop() {
    window->close();
}

void Game::switch_scene(const std::string& name) {
    if (scene)
        scene->disable();
    
    scene = scenes[name];

    scene->enable();

    if (scene->get_options().grab_mouse)
        window->grab_mouse(true);
    else
        window->grab_mouse(false);
}
template <class T>
void Game::add_scene(const std::string& name) {
    scenes[name] = new T();
    scenes[name]->init();
}

std::thread* Game::create_worker_thread() {
    return new std::thread([&]() {
        while (running) {
            if (scene) {
                scene->update_worker();
            }
        }
    });
}
