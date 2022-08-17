#include "game.hpp"

namespace game {
    void SceneGame::init() {
        handlers.game = this;
        handlers.listen();
        
        srand(time(0));

        d.world_seed = glm::linearRand(-(1<<16), 1>>16);

        d.world = std::make_shared<World>(new World(d.world_seed, Options::get("render_distance")));
    }

    void SceneGame::enable() {
        handlers.clear();
    }

    void SceneGame::update(float dt) {
        handlers.process();

        d.player.update(dt);

        d.world->update();
        d.world->generate(*r.block_textures);
    }
    void SceneGame::update_keyboard(float dt, const bool* pressed) {
        auto old_chunk_pos = wu::chunk_pos({d.player.pos.x, d.player.pos.z});

        char movement = 0;
        if (pressed[GLFW_KEY_W])
            movement |= (int)PlayerMoveDirection::FORWARD;
        if (pressed[GLFW_KEY_S])
            movement |= (int)PlayerMoveDirection::BACKWARD;
        if (pressed[GLFW_KEY_A])
            movement |= (int)PlayerMoveDirection::LEFT;
        if (pressed[GLFW_KEY_D])
            movement |= (int)PlayerMoveDirection::RIGHT;
        if (pressed[GLFW_KEY_SPACE])
            movement |= (int)PlayerMoveDirection::UP;
        if (pressed[GLFW_KEY_LEFT_SHIFT])
            movement |= (int)PlayerMoveDirection::DOWN;
        d.player.move(movement, dt * 40);

        auto chunk_pos = wu::chunk_pos({d.player.pos.x, d.player.pos.z});

        if (old_chunk_pos != chunk_pos) {
            d.world->move(d.player.pos.x, d.player.pos.z);
        }
    }
    void SceneGame::render() {
        Renderer::set_sky_color({0.47, 0.65, 1.0});

        r.block_shader->uniform("light.ambient", glm::vec3(0.3f));

        r.block_shader->uniform("light.diffuse", glm::vec3(0.7f));
        r.block_shader->uniform("light.diffuse_dir", glm::vec3(1.0f, -1.0f, 1.0f));

        r.block_shader->uniform("light.sun", glm::vec3(2.8f, 2.8f, 2.4f));

        handlers.option_change_event_queue.process();
        handlers.chunk_load_event_queue.process();
        handlers.chunk_unload_event_queue.process();

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
    void SceneGame::on_option_change(const EventOptionChange& e) {
        if (e.name == "render_distance" && d.world)
            d.world->set_render_distance(e.value);
    }
    void SceneGame::on_chunk_load(const EventChunkLoad& e) {
        d.chunk_meshes[{e.cx, e.cz}] = std::make_shared<Mesh>(new Mesh(
            *e.mesh_data,
            r.block_shader,
            r.block_textures_map,
            {e.cx * CHUNK_SIZE + 0.5f, 0.0f, e.cz * CHUNK_SIZE + 0.5f}
        ));
    }
    void SceneGame::on_chunk_unload(const EventChunkUnload& e) {
        d.chunk_meshes.erase({e.cx, e.cz});
    }
};
