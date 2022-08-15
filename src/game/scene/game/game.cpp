#include "game.hpp"

namespace game {
    void SceneGame::init() {
        handlers.game = this;
        handlers.listen();

        // d.chunks[{0, 0}] = std::make_shared<SceneGameChunk>(new SceneGameChunk(
        //     std::make_shared<Chunk>(new Chunk(
        //         4,
        //         0, 0
        //     ))
        // ));
        // auto mesh_data = d.chunks[{0, 0}]->chunk->mesh(*r.block_textures);
        // d.chunks[{0, 0}]->mesh = std::make_shared<Mesh>(new Mesh(
        //     *mesh_data,
        //     r.block_shader,
        //     r.block_textures_map,
        //     glm::vec3(0.5f, 0.0f, 0.0f)
        // ));
    }

    void SceneGame::enable() {
        handlers.clear();
    }

    void SceneGame::update(float dt) {
        handlers.process();

        d.player.update(dt);
    }
    void SceneGame::update_keyboard(float dt, const bool* pressed) {
        char movement = 0;
        if (pressed[GLFW_KEY_W])
            movement |= (int)CameraMoveDirection::FORWARD;
        if (pressed[GLFW_KEY_S])
            movement |= (int)CameraMoveDirection::BACKWARD;
        if (pressed[GLFW_KEY_A])
            movement |= (int)CameraMoveDirection::LEFT;
        if (pressed[GLFW_KEY_D])
            movement |= (int)CameraMoveDirection::RIGHT;
        if (pressed[GLFW_KEY_SPACE])
            movement |= (int)CameraMoveDirection::UP;
        if (pressed[GLFW_KEY_LEFT_SHIFT])
            movement |= (int)CameraMoveDirection::DOWN;
        d.player.move(movement, dt * 40);
    }
    void SceneGame::render() {
        Renderer::set_sky_color({0.47, 0.65, 1.0});

        for (auto& [pos, mesh] : d.chunk_meshes) {
            if (mesh)
                Renderer::render(mesh);
        }

        for (auto& [name, mesh] : d.gui_meshes) {
            Renderer::render(mesh, {false, false});
        }
    }

    SceneFlags SceneGame::get_options() {
        return {true};
    }

    void SceneGame::on_window_resize(const EventFramebufferResize& e) {
    }
    void SceneGame::on_mouse_move(const EventMouseMove& e) {
        d.player.rotate(e.x, e.y);
    }
    void SceneGame::on_mouse_click(const EventMouseClick& e) {
    }
    void SceneGame::on_key_press(const EventKeyPress& e) {
        if (e.key == GLFW_KEY_ESCAPE && e.action == GLFW_PRESS)
            EventManager::fire(EventSceneChange{"game:paused"});
    }
};
