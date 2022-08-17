#include "mesh.hpp"

Mesh::Mesh(Mesh* other) {
    shader = other->shader;
    VAO = other->VAO;
    VBO = other->VBO;
    for (auto& [name, tex] : other->textures) {
        textures[name] = tex;
    }
    vertices = other->vertices;
    translation_ = other->translation_;
    aabb = other->aabb;
}
Mesh::Mesh(
        const MeshData& data,
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures,
        const glm::vec3& translation,
        const frustum::AABB& aabb
    )
    : shader(shader), textures(textures), translation_(translation), aabb(aabb) {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    GLenum error;
    while ((error = glGetError())) {
        std::cout << "ERROR: " << error << "\n";
        asm("int $3");
    }

    rebuild(data);
}

Mesh& Mesh::operator=(const Mesh& other) {
    shader = other.shader;
    VAO = other.VAO;
    VBO = other.VBO;
    for (auto& [name, tex] : other.textures) {
        textures[name] = tex;
    }
    vertices = other.vertices;
    translation_ = other.translation_;
    aabb = other.aabb;
    return *this;
}

void Mesh::render(const RenderData& data) {
    int i = 0;
    for (auto& [name, texture] : textures) {
        texture->bind(i);
        shader->uniform(name, i);
        i++;
    }

    shader->uniform("transform", data.transform);
    shader->uniform("model", data.model);
    shader->uniform("shadow_transform", data.shadow_transform);
    shader->uniform("ortho", data.ortho);
    shader->uniform("shadowMap", data.shadow_map);
    shader->uniform("shadowMapEnabled", data.shadow_map_enabled);

    shader->use();
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, vertices);
}
void Mesh::render_shadows(const RenderData& data) {
    int i = 0;
    for (auto& [name, texture] : textures) {
        texture->bind(i);
        shader->shadow->uniform(name, i);
        i++;
    }

    shader->shadow->uniform("transform", data.transform);
    shader->shadow->uniform("model", data.model);

    shader->shadow->use();
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, vertices);
}

glm::vec3 Mesh::translation() const {
    return translation_;
}

bool Mesh::in_frustum(const frustum::Frustum& frustum) const {
    return aabb.in_frustum(frustum);
}

void Mesh::rebuild(const MeshData& data) {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.data.size() * sizeof(float), data.data.data(), GL_STATIC_DRAW);

    int stride = 0;
    for (auto& size : data.part_sizes) {
        stride += size;
    }

    int i = 0, cs = 0;
    for (auto& size : data.part_sizes) {
        glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)((long)(cs*sizeof(float))));
        glEnableVertexAttribArray(i);

        cs += size;
        i++;
    }

    vertices = data.data.size() / cs;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
