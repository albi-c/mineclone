#include "game/scene/game/game.hpp"
#include "camera/frustum.hpp"
#include "game/options.hpp"
#include "graphics/gui/imgui.hpp"
#include "util/thread.hpp"
#include "util/time.hpp"
#include "world/chunk.hpp"
#include "world/generation/old_generator.hpp"

#include <GLFW/glfw3.h>
#include <memory>
#include <sstream>
#include <string>

#define CROSSHAIR_SIZE 8.0f

namespace game {
    void SceneGame::init() {
        handlers.game = this;
        handlers.listen();
        
        srand(time(0));

        d.world_seed = glm::linearRand(-(1<<16), 1>>16);

        d.generator = std::make_shared<OldWorldGenerator>(d.world_seed);

        d.world = std::make_shared<World>(d.world_seed, r.block_textures, d.generator, Options::get("render_distance"));

        d.world->update_loaded();

        d.world->generate(*r.block_textures);

        d.player.pos = glm::vec3(0.0f, (float)(d.world->highest_block(0, 0) + 2), 0.0f);

        d.chunk_mesh_group = std::make_shared<MeshGroup<std::pair<int, int>>>(r.block_shader, r.block_textures_map);


        r.block_shader->use();

        Renderer::set_sky_color({0.47, 0.65, 1.0});

        r.block_shader->uniform_b("light.ambient", glm::vec3(0.3f));

        r.block_shader->uniform_b("light.diffuse", glm::vec3(0.7f));
        r.block_shader->uniform_b("light.diffuse_dir", glm::vec3(1.0f, -1.0f, 1.0f));

        r.block_shader->uniform_b("light.sun", glm::vec3(2.8f, 2.8f, 2.4f));

        r.block_shader->uniform_b("block_properties.plant", (int)MaterialProperty::PLANT);
        r.block_shader->uniform_b("block_properties.waving", (int)MaterialProperty::WAVING);
        for (int block = 0; block < Material::_size(); block++) {
            r.block_shader->uniform_b("block_properties.b[" + std::to_string(block) + "]", MATERIAL_PROPERTIES[block]);
        }

        set_render_distance(Options::get("render_distance"));
    }

    void SceneGame::enable() {
        handlers.clear();

        d.world->move(d.player.pos.x, d.player.pos.z);
    }

    void SceneGame::update(float dt) {
        handlers.process();

        d.player.update(dt);

        Camera::pos = d.player.get_eye_pos();
    }
    void SceneGame::update_worker() {
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

        if (pressed[GLFW_KEY_C])
            Camera::fov = 19.0f;
        else
            Camera::fov = 20.5f;

        auto chunk_pos = wu::chunk_pos({d.player.pos.x, d.player.pos.z});

        if (old_chunk_pos != chunk_pos) {
            d.world->move(d.player.pos.x, d.player.pos.z);
        }
    }
    void SceneGame::render() {
        static double last_time = 0.0;

        double time = glfwGetTime();
        double dt = time - last_time;
        double fps = 1.0 / dt;
        last_time = time;

        r.block_shader->use();
        r.block_shader->uniform_b("time", glfwGetTime());
        r.block_shader->uniform_b("camera_pos", Camera::pos);

        Timer timer_chunks("Chunks");

        handlers.window_resize_event_handler.process();
        handlers.option_change_event_queue.process();

        double chunksStart = glfwGetTime();
        double threshold = 0.00025 * Options::get("chunk_updates");
        while (glfwGetTime() - chunksStart < threshold) {
            Timer timer_upload("Chunk Upload");

            handlers.chunk_load_event_queue.process_one();

            timer_upload.stop();
        }
        handlers.chunk_unload_event_queue.process();

        timer_chunks.stop();

        Renderer::render(d.chunk_mesh_group);

        for (auto& [name, mesh] : d.gui_meshes) {
            Renderer::render(mesh, {false, false});
        }
        d.fps_history[d.fps_history_ptr++] = fps;
        if (d.fps_history_ptr >= FPS_HISTORY_LENGTH) {
            d.fps_history_ptr = 0;
        }

        int average_fps = 0;
        for (int i = 0; i < FPS_HISTORY_LENGTH; i++) {
            average_fps += d.fps_history[i];
        }
        average_fps /= FPS_HISTORY_LENGTH;

        // Debug GUI

        imgui::frame_start();

        {
            ImGui::Begin(
                "Debug",
                NULL,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings
            );

            std::stringstream debug_text;

            debug_text << "Pos: " << (int)(d.player.pos.x) << ", " << (int)(d.player.pos.y) << ", " << (int)(d.player.pos.z) << "\n";
            debug_text << "FPS: " << average_fps << "\n";
            debug_text << "Loaded chunks: " << d.chunk_mesh_group->num_meshes() << "\n";
            debug_text << "Generate queue: " << d.world->queued_chunks_generate() << "\n";
            debug_text << "Mesh queue: " << d.world->queued_chunks_mesh() << "\n";

            ImGui::TextScaled(debug_text.str(), 2);
        }

        imgui::frame_end();
    }

    SceneFlags SceneGame::get_options() {
        return {true};
    }

    void SceneGame::on_window_resize(const EventFramebufferResize& e) {
        float hw = e.width / 2.0f;
        float hh = e.height / 2.0f;

        d.gui_meshes["crosshair"] = std::make_shared<Mesh>(
            MeshData<float>(
                {
                    MeshDataPart(2),
                    MeshDataPart(2)
                },
                {
                    hw - CROSSHAIR_SIZE, hh - CROSSHAIR_SIZE,  0.0f, 0.0f,
                    hw - CROSSHAIR_SIZE, hh + CROSSHAIR_SIZE,  0.0f, 1.0f,
                    hw + CROSSHAIR_SIZE, hh + CROSSHAIR_SIZE,  1.0f, 1.0f,
                    hw - CROSSHAIR_SIZE, hh - CROSSHAIR_SIZE,  0.0f, 0.0f,
                    hw + CROSSHAIR_SIZE, hh + CROSSHAIR_SIZE,  1.0f, 1.0f,
                    hw + CROSSHAIR_SIZE, hh - CROSSHAIR_SIZE,  1.0f, 0.0f
                }
            ),
            r.crosshair_shader,
            r.crosshair_textures_map
        );
    }
    void SceneGame::on_mouse_move(const EventMouseMove& e) {
        d.player.rotate(e.x, e.y);
    }
    void SceneGame::on_mouse_click(const EventMouseClick& e) {
        if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
            Ray ray(Camera::pos, glm::normalize(Camera::front));
            d.world->raycast(ray, 5.0f);
            d.world->set(ray.position(), Material::AIR);
        } else if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
            Ray ray(Camera::pos, glm::normalize(Camera::front));
            if (d.world->raycast(ray, 5.0f) != Material::AIR) {
                ray.step(-0.03f);
                d.world->set(ray.position(), Material::GLASS);
            }
        }
    }
    void SceneGame::on_key_press(const EventKeyPress& e) {
        if (e.key == GLFW_KEY_ESCAPE && e.action == GLFW_PRESS)
            EventManager::fire(EventSceneChange{"game:paused"});
    }
    void SceneGame::on_option_change(const EventOptionChange& e) {
        if (e.name == "render_distance" && d.world) {
            d.world->set_render_distance(e.value);
            set_render_distance(e.value);
        }
    }
    void SceneGame::on_chunk_load(const EventChunkLoad& e) {
        if (d.world->loaded(e.cx, e.cz)) {
            auto mesh = std::make_shared<Mesh>(
                *e.mesh_data,
                r.block_shader,
                r.block_textures_map,
                glm::vec3(e.cx * CHUNK_SIZE + 0.5f, 0.0f, e.cz * CHUNK_SIZE + 0.5f),
                frustum::AABB(
                    {e.cx * CHUNK_SIZE + 0.5f, 0.0f, e.cz * CHUNK_SIZE + 0.5f},
                    {e.cx * CHUNK_SIZE + CHUNK_SIZE + 0.5f, CHUNK_HEIGHT, e.cz * CHUNK_SIZE + CHUNK_SIZE + 0.5f}
                )
            );

            (*d.chunk_mesh_group)[{e.cx, e.cz}] = mesh;
        }
    }
    void SceneGame::on_chunk_unload(const EventChunkUnload& e) {
        d.chunk_mesh_group->erase({e.cx, e.cz});
    }

    void SceneGame::set_render_distance(int distance) {
        r.block_shader->uniform("fog_start", (float) distance * CHUNK_SIZE - CHUNK_SIZE);
    }
};
