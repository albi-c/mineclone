#pragma once

#include <map>
#include <vector>

#include "lib/opengl.hpp"
#include "graphics/shaders/shader.hpp"
#include "graphics/textures/texture.hpp"
#include "graphics/textures/texture_array.hpp"

struct MeshData {
    std::vector<int> part_sizes;
    std::vector<float> data;

    MeshData() {}
    MeshData(std::vector<int> part_sizes, std::vector<float> data)
        : part_sizes(part_sizes), data(data) {}
};

class Mesh {
public:
    Shader* shader;
    Shader shader_shadow;

    Mesh(const MeshData& data, Shader* shader, const std::map<std::string, Texture*>& textures, const std::map<std::string, TextureArray*>& textureArrays);

    void render();
    void render_shadows();

    void rebuild(const MeshData& data);

private:
    GLuint VAO, VBO;
    std::map<std::string, Texture*> textures;
    std::map<std::string, TextureArray*> textureArrays;
    int vertices;
};
