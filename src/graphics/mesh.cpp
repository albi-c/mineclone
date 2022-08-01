#include "mesh.hpp"

Mesh::Mesh()
    : shader_shadow(BuiltinShader::DEPTH) {}
Mesh::Mesh(const MeshData& data, Shader* shader, const std::map<std::string, Texture*>& textures, const std::map<std::string, Texture3D*>& textures3d)
    : shader(shader), textures(textures), textures3d(textures3d), shader_shadow(BuiltinShader::DEPTH) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    rebuild(data);
}

void Mesh::render() {
    int i = 0;
    for (auto& [name, texture] : textures) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        shader->uniform(name, i);
        i++;
    }
    for (auto& [name, texture] : textures3d) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture->texture);
        shader->uniform(name, i);
        i++;
    }

    shader->use();
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, vertices);
}
void Mesh::render_shadows() {
    int i = 0;
    for (auto& [name, texture] : textures) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        shader_shadow.uniform(name, i);
        i++;
    }
    for (auto& [name, texture] : textures3d) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture->texture);
        shader_shadow.uniform(name, i);
        i++;
    }

    shader_shadow.use();
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, vertices);
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
