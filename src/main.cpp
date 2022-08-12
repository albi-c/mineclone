#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <utility>
#include <thread>
#include <utility>
#include <filesystem>
namespace fs = std::filesystem;

#include "window.hpp"
#include "graphics/texture.hpp"
#include "world/world.hpp"
#include "world/ray.hpp"
#include "event/event.hpp"

bool break_block_g = false;
void mouse_click_callback(const Event& e) {
    if ((long)e.data == GLFW_MOUSE_BUTTON_LEFT) {
        break_block_g = true;
    }
}

std::set<std::pair<int, int>> chunk_redraw_pos_g;
void chunk_redraw_callback(const Event& e) {
    auto data = (unsigned long)e.data;

    chunk_redraw_pos_g.insert({(data & 0xffffffff00000000) >> 32, data & 0xffffffff});
}

int main() {
    srand(time(0));
    int seed = glm::linearRand<int>(-(2<<15), 2>>15);
    seed = 4;
    srand(seed);

    Event::listen(EventType::MouseClick, mouse_click_callback);
    Event::listen(EventType::ChunkRedraw, chunk_redraw_callback);

    Window w;

    w.renderer.set_sky_color(glm::vec4(0.47, 0.65, 1.0, .0));

    w.camera.pos.y = 70;

    Shader shader(BuiltinShader::BLOCK);
    shader.uniform("light.ambient", glm::vec3(0.3f));
    shader.uniform("light.diffuse", glm::vec3(0.7f));
    shader.uniform("light.diffuse_pos", glm::vec3(2.0f, 120.0f, 2.0f));

    Texture3D tex3d;
    for (auto& path_ : fs::directory_iterator("res/tex")) {
        std::string path = path_.path();
        std::string path_u = path;
        std::transform(path_u.begin(), path_u.end(), path_u.begin(), ::toupper);
        tex3d.add(path_u.substr(8, path.length() - 12), path);
    }
    tex3d.generate();

    std::map<std::string, Texture3D*> textures;
    textures["textureArray"] = &tex3d;

    Shader gui_shader(BuiltinShader::GUI);

    Texture crosshair_tex("res/crosshair.png");
    std::map<std::string, Texture*> crosshair_textures;
    crosshair_textures["texture1"] = &crosshair_tex;
    Mesh crosshair(MeshData({2, 2}, {
        952.0f, 481.5f,  0.0f, 0.0f,
        952.0f, 497.5f,  0.0f, 1.0f,
        968.0f, 497.5f,  1.0f, 1.0f,
        952.0f, 481.5f,  0.0f, 0.0f,
        968.0f, 497.5f,  1.0f, 1.0f,
        968.0f, 481.5f,  1.0f, 0.0f
    }), &gui_shader, crosshair_textures, {});
    // Mesh crosshair(MeshData({2, 2}, {
    //     -0.5f, -0.5f,  0.0f, 0.0f,
    //     -0.5f,  0.5f,  0.0f, 1.0f,
    //      0.5f,  0.5f,  1.0f, 1.0f,
    //     -0.5f, -0.5f,  0.0f, 0.0f,
    //      0.5f,  0.5f,  1.0f, 1.0f,
    //      0.5f, -0.5f,  1.0f, 0.0f
    // }), &gui_shader, crosshair_textures, {});

    w.renderer.add_mesh(0, &crosshair, glm::vec3(0.0f));

    const int n_chunks = 4;
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

            meshes[i][j] = new Mesh(chunks[i][j]->mesh(&tex3d), &shader, {}, textures);
            w.renderer.add_mesh((i + j * n_chunks + 1) << 8, meshes[i][j], {i * 16 + 0.5, 0, j * 16 + 0.5});
        }
    }

    w.camera.pos.x = n_chunks / 2 * CHUNK_SIZE;
    w.camera.pos.z = n_chunks / 2 * CHUNK_SIZE;

    std::map<std::pair<int, int>, MeshData> generated_chunk_meshes;

    bool worker_thread1_running = true;
    std::thread worker_thread1([&]() {
        while (worker_thread1_running) {
            if (break_block_g) {
                break_block_g = false;

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
            }

            for (int i = 0; i < n_chunks; i++) {
                for (int j = 0; j < n_chunks; j++) {
                    chunks[i][j]->update();
                }
            }

            for (auto& [x, z] : chunk_redraw_pos_g) {
                if (x < 0 || z < 0 || x >= n_chunks || z >= n_chunks)
                    continue;
                
                generated_chunk_meshes[{x, z}] = chunks[x][z]->mesh(&tex3d);
            }
            chunk_redraw_pos_g.clear();
        }
    });

    w.grab_mouse(true);
    while (w.update()) {
        w.render();
        if (w.get_key(GLFW_KEY_ESCAPE)) {
            w.close();
        }

        for (auto& [pos, mesh] : generated_chunk_meshes) {
            delete meshes[pos.first][pos.second];
            meshes[pos.first][pos.second] = new Mesh(mesh, &shader, {}, textures);
            w.renderer.add_mesh(pos.first + pos.second * n_chunks, meshes[pos.first][pos.second], {pos.first * 16 + 0.5, 0, pos.second * 16 + 0.5});
        }
        generated_chunk_meshes.clear();
    }

    worker_thread1_running = false;
    worker_thread1.join();

    return 0;
}
