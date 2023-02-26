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

template <class T>
struct MeshData {
    std::vector<MeshDataPart> parts;
    std::vector<T> data;

    MeshData() {}
    MeshData(const std::vector<MeshDataPart>& parts, const std::vector<T>& data)
        : parts(parts), data(data) {}
};

class Mesh : public Renderable {
public:
    template <class T>
    Mesh(
        const MeshData<T>& data,
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures,
        const glm::vec3& translation = glm::vec3(0.0f),
        const frustum::AABB& aabb = frustum::AABB()
    );
    ~Mesh();

    Mesh& operator=(const Mesh& other);

    void render(const RenderData& data) override;
    void render_shadows(const RenderData& data) override;

    void render_basic();

    glm::vec3 translation() const override;

    bool in_frustum(const frustum::Frustum& frustum) override;

    template <class T>
    void rebuild(const MeshData<T>& data);

private:
    GLuint VAO, VBO;
    bool buffers_initialized = false;
    std::shared_ptr<Shader> shader;
    std::map<std::string, std::shared_ptr<Texture>> textures;
    int vertices;
    glm::vec3 translation_;
    frustum::AABB aabb;
};

template <class T>
inline Mesh::Mesh(
        const MeshData<T>& data,
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures,
        const glm::vec3& translation,
        const frustum::AABB& aabb
    )
    : shader(shader), textures(textures), translation_(translation), aabb(aabb) {

    rebuild(data);
}

template <class T>
inline void Mesh::rebuild(const MeshData<T>& data) {
    if (buffers_initialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.data.size() * sizeof(T), data.data.data(), GL_STATIC_DRAW);

    int stride = 0;
    for (auto& part : data.parts) {
        stride += part.size;
    }

    int i = 0, cs = 0;
    for (auto& part : data.parts) {
        glVertexAttribPointer(i, part.size, part.type, GL_FALSE, stride * sizeof(T), (void*)((long)(cs*sizeof(T))));
        glEnableVertexAttribArray(i);

        cs += part.size;
        i++;
    }

    vertices = data.data.size() / cs;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
