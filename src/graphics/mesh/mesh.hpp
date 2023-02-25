#pragma once

#include <map>
#include <vector>
#include <memory>

#include "lib/opengl.hpp"
#include "graphics/shaders/shader.hpp"
#include "graphics/textures/texture.hpp"
#include "graphics/render/renderable.hpp"
#include "camera/frustum.hpp"

struct MeshDataPart {
    const unsigned int size;
    const GLuint type;

    MeshDataPart(unsigned int size)
        : size(size), type(GL_FLOAT) {}
    MeshDataPart(unsigned int size, GLuint type)
        : size(size), type(type) {}
};

struct MeshData {
    std::vector<MeshDataPart> parts;
    std::vector<float> data;

    MeshData() {}
    MeshData(const std::vector<MeshDataPart>& parts, const std::vector<float>& data)
        : parts(parts), data(data) {}
};

class Mesh : public Renderable {
public:
    Mesh(
        const MeshData& data,
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures,
        const glm::vec3& translation = glm::vec3(0.0f),
        const frustum::AABB& aabb = frustum::AABB()
    );

    Mesh& operator=(const Mesh& other);

    void render(const RenderData& data) override;
    void render_shadows(const RenderData& data) override;

    void render_basic();

    glm::vec3 translation() const override;

    bool in_frustum(const frustum::Frustum& frustum) override;

    void rebuild(const MeshData& data);

private:
    GLuint VAO, VBO;
    bool buffers_initialized = false;
    std::shared_ptr<Shader> shader;
    std::map<std::string, std::shared_ptr<Texture>> textures;
    int vertices;
    glm::vec3 translation_;
    frustum::AABB aabb;
};
