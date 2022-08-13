#pragma once

#include <map>
#include <vector>
#include <memory>

#include "lib/opengl.hpp"
#include "graphics/shaders/shader.hpp"
#include "graphics/textures/texture.hpp"
#include "graphics/textures/texture_array.hpp"
#include "graphics/render/renderable.hpp"

struct MeshData {
    std::vector<int> part_sizes;
    std::vector<float> data;

    MeshData() {}
    MeshData(std::vector<int> part_sizes, std::vector<float> data)
        : part_sizes(part_sizes), data(data) {}
};

class Mesh : public Renderable {
public:
    std::shared_ptr<Shader> shader;
    Shader shader_shadow;

    Mesh(
        const MeshData& data,
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures
    );

    void render(const std::map<std::string, Uniform>& uniforms) override;
    void render_shadows(const std::map<std::string, Uniform>& uniforms) override;

    std::vector<std::string> required_textures() const override;

    void rebuild(const MeshData& data);

private:
    GLuint VAO, VBO;
    std::map<std::string, std::shared_ptr<Texture>> textures;
    int vertices;
};
