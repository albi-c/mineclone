#include "mesh.hpp"

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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

    render_basic();
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
