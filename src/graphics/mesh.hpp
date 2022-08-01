#pragma once

#include <map>
#include <vector>

#include "opengl.hpp"
#include "shader.hpp"
#include "texture.hpp"

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

    Mesh();
    Mesh(const MeshData& data, Shader* shader, const std::map<std::string, Texture*>& textures, const std::map<std::string, Texture3D*>& texture3d);

    void render();
    void render_shadows();

    void rebuild(const MeshData& data);

private:
    GLuint VAO, VBO;
    std::map<std::string, Texture*> textures;
    std::map<std::string, Texture3D*> textures3d;
    int vertices;
};
