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
    Window w;

    w.renderer.set_sky_color(glm::vec4(0.47, 0.65, 1.0, .0));

    w.camera.pos.y = 60;

    Shader shader(BuiltinShader::BLOCK);

    shader.uniform("texture_mix", 0.7f);

    TextureAtlas atlas;
    atlas.add("STONE", "res/stone.png");
    atlas.add("DIRT", "res/dirt.png");
    atlas.add("GRASS_BLOCK_SIDE", "res/grass_block_side.png");
    atlas.add("GRASS_BLOCK_TOP", "res/grass_block_top.png");
    atlas.add("LOG", "res/oak_log.png");

    Texture3D tex3d;
    for (auto& path_ : fs::directory_iterator("res")) {
        std::string path = path_.path();
        std::string path_u = path;
        std::transform(path_u.begin(), path_u.end(), path_u.begin(), ::toupper);
        tex3d.add(path_u.substr(4, path.length() - 8), path);
    }
    tex3d.generate();

    std::map<std::string, Texture3D*> textures;
    textures["texture1"] = &tex3d;

    Chunk chunk(0, 0);

    double total = 0.0;
    for (int i = 0; i < 100; i++) {
        double time1 = glfwGetTime();
        chunk.mesh(&tex3d);
        double time2 = glfwGetTime();
        total += time2 - time1;
    }
    std::cout << total << " | " << total / 100.0 * 1000.0 << "\n";

    Mesh mesh(chunk.mesh(&tex3d), &shader, {}, textures);
    w.renderer.add_mesh(0, &mesh);

    w.grab_mouse(true);

    while (w.update()) {
        w.render();
        if (w.get_key(GLFW_KEY_ESCAPE)) {
            w.close();
        }
    }

    return 0;
}
