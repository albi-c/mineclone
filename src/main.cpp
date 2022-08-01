#include <iostream>
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

#include "window.hpp"
#include "graphics/texture.hpp"
#include "world/chunk.hpp"

static const std::vector<float> shape_data = {
    // positions          // colors           // texture coords
    -0.5f, -0.5f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left 
     0.5f,  0.5f, 1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right

    -0.5f, -0.5f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
     0.5f,  0.5f, -1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right  
};

int main() {
    srand(time(0));
    int seed = glm::linearRand<int>(-(2<<15), 2>>15);
    srand(seed);

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

    const int n_chunks = 8;
    Chunk* chunks[n_chunks][n_chunks];
    Mesh* meshes[n_chunks][n_chunks];
    for (int i = 0; i < n_chunks; i++) {
        for (int j = 0; j < n_chunks; j++) {
            chunks[i][j] = new Chunk(seed, i, j);
            meshes[i][j] = new Mesh(chunks[i][j]->mesh(&tex3d), &shader, {}, textures);
            w.renderer.add_mesh(i + j * n_chunks, meshes[i][j], {i * 16, 0, j * 16});
        }
    }

    w.camera.pos.x = n_chunks / 2 * 16;
    w.camera.pos.z = n_chunks / 2 * 16;

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
    }

    return 0;
}
