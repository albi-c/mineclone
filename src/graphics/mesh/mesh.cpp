#include "graphics/mesh/mesh.hpp"

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Mesh::render(const RenderData& data) {
    shader->use();

    int i = 0;
    for (auto& [name, texture] : textures) {
        texture->bind(i);
        shader->uniform_b(name, i);
        i++;
    }

    shader->uniform_b("transform", data.transform);
    shader->uniform_b("model", data.model);
    shader->uniform_b("shadow_transform", data.shadow_transform);
    shader->uniform_b("ortho", data.ortho);
    shader->uniform_b("shadowMap", data.shadow_map);
    shader->uniform_b("shadowMapEnabled", data.shadow_map_enabled);

    render_basic();
}
void Mesh::render_shadows(const RenderData& data) {
    shader->shadow->use();

    int i = 0;
    for (auto& [name, texture] : textures) {
        texture->bind(i);
        shader->shadow->uniform_b(name, i);
        i++;
    }

    shader->shadow->uniform_b("transform", data.transform);
    shader->shadow->uniform_b("model", data.model);
    
    render_basic();
}

void Mesh::render_basic() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices);
}

glm::vec3 Mesh::translation() const {
    return translation_;
}

bool Mesh::in_frustum(const frustum::Frustum& frustum) {
    return aabb.in_frustum(frustum);
}
