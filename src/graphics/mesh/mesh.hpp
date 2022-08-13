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

    Mesh(Mesh* other);
    Mesh(
        const MeshData& data,
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures,
        const glm::vec3& translation = glm::vec3(0.0f)
    );

    void render(const RenderData& data) override;
    void render_shadows(const RenderData& data) override;

    glm::vec3 translation() const override;

    void rebuild(const MeshData& data);

private:
    GLuint VAO, VBO;
    std::map<std::string, std::shared_ptr<Texture>> textures;
    int vertices;
    glm::vec3 translation_;
};
