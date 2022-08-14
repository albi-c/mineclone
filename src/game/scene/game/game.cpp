#include "game.hpp"

namespace game {
    void SceneGame::init() {
        handlers.game = this;
        handlers.listen();

        d.chunks[{0, 0}] = std::make_shared<SceneGameChunk>(new SceneGameChunk(
            std::make_shared<Chunk>(new Chunk(
                4,
                0, 0
            ))
        ));
        MeshData mesh_data = d.chunks[{0, 0}]->chunk->mesh(*r.block_textures);
        d.chunks[{0, 0}]->mesh = std::make_shared<Mesh>(new Mesh(
            mesh_data,
            r.block_shader,
            r.block_textures_map,
            glm::vec3(0.5f, 0.0f, 0.0f)
        ));

        d.gui_meshes["test"] = std::make_shared<Mesh>(new Mesh(
            {
                {2, 3},
                {
                    -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
                     0.0f,  0.5f,  0.0f, 1.0f, 0.0f,
                     0.5f, -0.5f,  0.0f, 0.0f, 1.0f
                }
            },
            r.gui_shader,
            {}
        ));
    }

    void SceneGame::update(float dt) {
        handlers.process();

        d.player.update(dt);
    }
    void SceneGame::render() {
        Renderer::set_sky_color({0.47, 0.65, 1.0});

        for (auto& [pos, chunk] : d.chunks) {
            if (chunk->mesh)
                Renderer::render(chunk->mesh);
        }

        for (auto& [name, mesh] : d.gui_meshes) {
            Renderer::render(mesh, {false, false});
        }
    }

    void SceneGame::on_mouse_move(const EventMouseMove& e) {
    }
    void SceneGame::on_mouse_click(const EventMouseClick& e) {
    }
};
