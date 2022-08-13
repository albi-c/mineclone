#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <utility>
#include <thread>
#include <utility>
#include <mutex>
#include <filesystem>
namespace fs = std::filesystem;

#include "window/window.hpp"
#include "graphics/textures/texture.hpp"
#include "graphics/textures/texture_array.hpp"
#include "world/chunk.hpp"
#include "util/ray.hpp"
#include "event/event.hpp"
#include "util/string.hpp"

// TESTING
#include "resources/loader.hpp"

#define CHUNK_MESH_ID(x, z, n_chunks) (((x) + (z) * (n_chunks) + 1) << 8)

int main() {
    srand(time(0));
    int seed = glm::linearRand<int>(-(2<<15), 2>>15);
    seed = 4;
    srand(seed);

    Window w;

    w.renderer.set_sky_color(glm::vec4(0.47, 0.65, 1.0, .0));

    w.camera.pos.y = 70;

    auto shader = std::make_shared<Shader>(new Shader(BuiltinShader::BLOCK));
    shader->uniform("light.ambient", glm::vec3(0.3f));
    shader->uniform("light.diffuse", glm::vec3(0.7f));
    shader->uniform("light.diffuse_pos", glm::vec3(2.0f, 120.0f, 2.0f));

    auto texArr = ResourceLoader::texture_array("minecraft", true);
    texArr->generate();

    std::map<std::string, std::shared_ptr<Texture>> textures;
    textures["textureArray"] = texArr;

    auto color_shader = std::make_shared<Shader>(new Shader(BuiltinShader::COLOR2D));

    Mesh crosshair(MeshData({2, 3}, {
        -0.5f,  0.0f,   1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
         0.5f,  0.0f,   1.0f, 0.0f, 0.0f
    }), color_shader, {});

    w.renderer.add_mesh(0, &crosshair);

    // auto crosshair_tex = ResourceLoader::texture("crosshair");
    // std::map<std::string, std::shared_ptr<Texture>> crosshair_textures;
    // crosshair_textures["texture1"] = crosshair_tex;
    // Mesh crosshair(MeshData({2, 2}, {
    //     952.0f, 481.5f,  0.0f, 0.0f,
    //     952.0f, 497.5f,  0.0f, 1.0f,
    //     968.0f, 497.5f,  1.0f, 1.0f,
    //     952.0f, 481.5f,  0.0f, 0.0f,
    //     968.0f, 497.5f,  1.0f, 1.0f,
    //     968.0f, 481.5f,  1.0f, 0.0f
    // }), gui_shader, crosshair_textures);
    // // Mesh crosshair(MeshData({2, 2}, {
    // //     -0.5f, -0.5f,  0.0f, 0.0f,
    // //     -0.5f,  0.5f,  0.0f, 1.0f,
    // //      0.5f,  0.5f,  1.0f, 1.0f,
    // //     -0.5f, -0.5f,  0.0f, 0.0f,
    // //      0.5f,  0.5f,  1.0f, 1.0f,
    // //      0.5f, -0.5f,  1.0f, 0.0f
    // // }), &gui_shader, crosshair_textures, {});

    const int n_chunks = 16;
    Chunk* chunks[n_chunks][n_chunks];
    Mesh* meshes[n_chunks][n_chunks];
    for (int i = 0; i < n_chunks; i++) {
        for (int j = 0; j < n_chunks; j++) {
            chunks[i][j] = new Chunk(seed, i, j);
        }
    }
    for (int i = 0; i < n_chunks; i++) {
        for (int j = 0; j < n_chunks; j++) {
            if (i > 0)
                chunks[i][j]->set_neighbor(ChunkNeighbor::NX, chunks[i-1][j]);
            if (i < n_chunks - 1)
                chunks[i][j]->set_neighbor(ChunkNeighbor::PX, chunks[i+1][j]);
            if (j > 0)
                chunks[i][j]->set_neighbor(ChunkNeighbor::NZ, chunks[i][j-1]);
            if (j < n_chunks - 1)
                chunks[i][j]->set_neighbor(ChunkNeighbor::PZ, chunks[i][j+1]);
            
            chunks[i][j]->update();

            meshes[i][j] = new Mesh(chunks[i][j]->mesh(*texArr), shader, textures);
            w.renderer.add_mesh(CHUNK_MESH_ID(i, j, n_chunks), meshes[i][j], {i * 16 + 0.5, 0, j * 16 + 0.5});
        }
    }

    w.camera.pos.x = n_chunks / 2 * CHUNK_SIZE;
    w.camera.pos.z = n_chunks / 2 * CHUNK_SIZE;

    std::map<std::pair<int, int>, MeshData> generated_chunk_meshes;
    std::mutex generated_chunk_meshes_mutex;

    bool worker_thread1_running = true;
    std::thread worker_thread1([&]() {
        FunctionEventQueue<EventMouseClick> mouse_click_event_queue([&](const EventMouseClick& e) {
            if (e.action == GLFW_PRESS) {
                if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
                    Ray ray(w.camera.pos, w.camera.front);
                    glm::vec<4, int> bounds = glm::vec<4, int>(0, 0, n_chunks * CHUNK_SIZE, n_chunks * CHUNK_SIZE);
                    while (true) {
                        ray.step(0.01);

                        glm::vec3 pos = ray.position();
                        if (pos.x < bounds.x || pos.z < bounds.y || pos.x > bounds.z || pos.z > bounds.w || pos.y < 0 || pos.y > CHUNK_HEIGHT)
                            break;
                        
                        if (chunks[(int)pos.x/CHUNK_SIZE][(int)pos.z/CHUNK_SIZE]->get((int)pos.x % 16, pos.y, (int)pos.z % 16) != Material::AIR) {
                            chunks[(int)pos.x/CHUNK_SIZE][(int)pos.z/CHUNK_SIZE]->set((int)pos.x % 16, pos.y, (int)pos.z % 16, Material::AIR);
                            break;
                        }
                    }
                } else if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
                    Ray ray(w.camera.pos, w.camera.front);
                    glm::vec<4, int> bounds = glm::vec<4, int>(0, 0, n_chunks * CHUNK_SIZE, n_chunks * CHUNK_SIZE);
                    while (true) {
                        ray.step(0.01);

                        glm::vec3 pos = ray.position();
                        if (pos.x < bounds.x || pos.z < bounds.y || pos.x > bounds.z || pos.z > bounds.w || pos.y < 0 || pos.y > CHUNK_HEIGHT)
                            break;
                        
                        if (chunks[(int)pos.x/CHUNK_SIZE][(int)pos.z/CHUNK_SIZE]->get((int)pos.x % 16, pos.y, (int)pos.z % 16) != Material::AIR) {
                            ray.step(-0.01);
                            pos = ray.position();
                            chunks[(int)pos.x/CHUNK_SIZE][(int)pos.z/CHUNK_SIZE]->set((int)pos.x % 16, pos.y, (int)pos.z % 16, Material::GLASS);
                            break;
                        }
                    }
                }
            }
        });
        EventManager::listen(mouse_click_event_queue);

        FunctionEventQueue<EventChunkRedraw> chunk_redraw_event_queue([&](const EventChunkRedraw& e) {
            if (e.cx < 0 || e.cz < 0 || e.cx >= n_chunks || e.cz >= n_chunks)
                return;
            
            MeshData data = chunks[e.cx][e.cz]->mesh(*texArr);
            
            generated_chunk_meshes_mutex.lock();

            generated_chunk_meshes[{e.cx, e.cz}] = data;

            generated_chunk_meshes_mutex.unlock();
        });
        EventManager::listen(chunk_redraw_event_queue);

        while (worker_thread1_running) {
            mouse_click_event_queue.process_one();
            chunk_redraw_event_queue.process_one();

            for (int i = 0; i < n_chunks; i++) {
                for (int j = 0; j < n_chunks; j++) {
                    chunks[i][j]->update();
                }
            }
        }
    });

    w.grab_mouse(true);
    while (w.update()) {
        w.render();
        // w.renderer.render2d(crosshair);
        if (w.get_key(GLFW_KEY_ESCAPE)) {
            w.close();
        }

        generated_chunk_meshes_mutex.lock();

        for (auto& [pos, mesh] : generated_chunk_meshes) {
            delete meshes[pos.first][pos.second];
            meshes[pos.first][pos.second] = new Mesh(mesh, shader, textures);
            w.renderer.add_mesh(CHUNK_MESH_ID(pos.first, pos.second, n_chunks), meshes[pos.first][pos.second], {pos.first * 16 + 0.5, 0, pos.second * 16 + 0.5});
        }
        generated_chunk_meshes.clear();

        generated_chunk_meshes_mutex.unlock();
    }

    worker_thread1_running = false;
    worker_thread1.join();

    return 0;
}
