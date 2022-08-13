#include "mesh.hpp"

Mesh::Mesh(
        const MeshData& data,
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures
    )
    : shader(shader), textures(textures), shader_shadow(BuiltinShader::DEPTH) {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    rebuild(data);
}

void Mesh::render(const std::map<std::string, Uniform>& uniforms) {
    int i = 0;
    for (auto& [name, texture] : textures) {
        texture->bind(i);
        shader->uniform(name, i);
        i++;
    }

    for (auto& [name, uniform] : uniforms) {
        uniform.set(*shader, name);
    }

    shader->use();
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, vertices);
}
void Mesh::render_shadows(const std::map<std::string, Uniform>& uniforms) {
    int i = 0;
    for (auto& [name, texture] : textures) {
        texture->bind(i);
        shader->uniform(name, i);
        i++;
    }

    for (auto& [name, uniform] : uniforms) {
        uniform.set(*shader, name);
    }

    shader_shadow.use();
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, vertices);
}

std::vector<std::string> Mesh::required_textures() const {
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
