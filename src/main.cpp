#include <iostream>
#include <algorithm>
#include <set>
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

// int chunk_redraw_pos_g[2] = {-1, -1};
std::set<std::pair<int, int>> chunk_redraw_pos_g;
void chunk_redraw_callback(const Event& e) {
    // chunk_redraw_pos_g[0] = ((int*)e.data)[0];
    // chunk_redraw_pos_g[1] = ((int*)e.data)[1];
    chunk_redraw_pos_g.insert({((int*)e.data)[0], ((int*)e.data)[1]});
}

int main() {
    srand(time(0));
    int seed = glm::linearRand<int>(-(2<<15), 2>>15);
    seed = 2;
    srand(seed);

    Event::listen(EventType::MouseClick, mouse_click_callback);
    Event::listen(EventType::ChunkRedraw, chunk_redraw_callback);

    Window w;

    w.renderer.set_sky_color(glm::vec4(0.47, 0.65, 1.0, .0));

    w.camera.pos.y = 70;

    Shader shader(BuiltinShader::BLOCK);
    shader.uniform("light.ambient", glm::vec3(0.3f));
    shader.uniform("light.diffuse", glm::vec3(1.1f));
    shader.uniform("light.diffuse_pos", glm::vec3(100.0f, 165.0f, 10.0f));

    Texture3D tex3d;
    for (auto& path_ : fs::directory_iterator("res")) {
        std::string path = path_.path();
        std::string path_u = path;
        std::transform(path_u.begin(), path_u.end(), path_u.begin(), ::toupper);
        tex3d.add(path_u.substr(4, path.length() - 8), path);
    }
    tex3d.generate();

    std::map<std::string, Texture3D*> textures;
    textures["textureArray"] = &tex3d;

    const int n_chunks = 16;
    Chunk* chunks[n_chunks][n_chunks];
    Mesh* meshes[n_chunks][n_chunks];
    for (int i = 0; i < n_chunks; i++) {
        for (int j = 0; j < n_chunks; j++) {
            chunks[i][j] = new Chunk(seed, i, j);
            meshes[i][j] = new Mesh(chunks[i][j]->mesh(&tex3d), &shader, {}, textures);
            w.renderer.add_mesh(i + j * n_chunks, meshes[i][j], {i * 16 + 0.5, 0, j * 16 + 0.5});
        }
    }

    w.camera.pos.x = n_chunks / 2 * CHUNK_SIZE;
    w.camera.pos.z = n_chunks / 2 * CHUNK_SIZE;

    w.grab_mouse(true);
    while (w.update()) {
        w.render();
        if (w.get_key(GLFW_KEY_ESCAPE)) {
            w.close();
        }

        for (int i = 0; i < n_chunks; i++) {
            for (int j = 0; j < n_chunks; j++) {
                chunks[i][j]->update();
            }
        }

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

        for (auto& [x, y] : chunk_redraw_pos_g) {
            delete meshes[x][y];
            meshes[x][y] = new Mesh(chunks[x][y]->mesh(&tex3d), &shader, {}, textures);
            w.renderer.add_mesh(x + y * n_chunks, meshes[x][y], {x * 16 + 0.5, 0, y * 16 + 0.5});
        }
        chunk_redraw_pos_g.clear();
    }

    return 0;
}
