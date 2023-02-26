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
    Mesh(
        const std::vector<MeshDataPart>& parts,
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures,
        size_t value_size,
        const glm::vec3& translation = glm::vec3(0.0f),
        const frustum::AABB& aabb = frustum::AABB()
    );
    ~Mesh();

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
    int cs = 0;

    template <class T>
    void set_data(const MeshData<T>& data, bool bound);
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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int stride = 0;
    for (auto& part : data.parts) {
        stride += part.size;
    }

    int i = 0;
    for (auto& part : data.parts) {
        glVertexAttribPointer(i, part.size, part.type, GL_FALSE, stride * sizeof(T), (void*)((long)(cs*sizeof(T))));
        glEnableVertexAttribArray(i);

        cs += part.size;
        i++;
    }

    glBindVertexArray(0);

    set_data(data, true);
}
inline Mesh::Mesh(
    const std::vector<MeshDataPart>& parts,
    std::shared_ptr<Shader> shader,
    const std::map<std::string, std::shared_ptr<Texture>>& textures,
    size_t value_size,
    const glm::vec3& translation,
    const frustum::AABB& aabb
) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int stride = 0;
    for (auto& part : parts) {
        stride += part.size;
    }

    int i = 0;
    for (auto& part : parts) {
        glVertexAttribPointer(i, part.size, part.type, GL_FALSE, stride * value_size, (void*)((long)(cs * value_size)));
        glEnableVertexAttribArray(i);

        cs += part.size;
        i++;
    }

    vertices = 0;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template <class T>
inline void Mesh::rebuild(const MeshData<T>& data) {
    set_data(data, false);
}

template <class T>
inline void Mesh::set_data(const MeshData<T>& data, bool bound) {
    if (!bound) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }

    glBufferData(GL_ARRAY_BUFFER, data.data.size() * sizeof(T), data.data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vertices = data.data.size() / cs;
}
