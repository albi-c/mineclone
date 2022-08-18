#pragma once

#include <map>
#include <memory>

#include "mesh.hpp"
#include "graphics/shaders/shader.hpp"
#include "graphics/textures/texture.hpp"
#include "graphics/render/renderable.hpp"

template <class T>
class MeshGroup : public Renderable {
public:
    inline MeshGroup(MeshGroup* other)
        : meshes(other->meshes) {}
    inline MeshGroup(
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures
    )
        : shader(shader), textures(textures) {}

    inline std::shared_ptr<Mesh>& operator[](const T& id) {
        return meshes[T];
    }
    inline void erase(const T& id) {
        meshes.erase(T);
    }
    inline bool contains(const T& id) {
        return meshes.find(T) != meshes.end();
    }

    inline glm::vec3 translation() const override {
        return glm::vec3(0.0f);
    }

    inline bool in_frustum(const frustum::Frustum& frustum) const override {
        this->frustum = &frustum;
    }

    inline void render(const RenderData& data) override {
        int i = 0;
        for (auto& [name, texture] : textures) {
            texture->bind(i);
            shader->uniform(name, i);
            i++;
        }

        shader->uniform("ortho", data.ortho);
        shader->uniform("shadowMap", data.shadow_map);
        shader->uniform("shadowMapEnabled", data.shadow_map_enabled);

        shader->use();

        for (auto& [id, mesh] : meshes) {
            if (frustum != nullptr && mesh->in_frustum(frustum)) {
                shader->uniform("transform", glm::translate(data.transform, mesh->translation());
                shader->uniform("model", glm::translate(data.model, mesh->translation());
                shader->uniform("shadow_transform", glm::translate(data.shadow_transform, mesh->translation());

                glBindVertexArray(mesh->VAO);
                glDrawArrays(GL_TRIANGLES, 0, mesh->vertices);
            }
        }

        frustum = nullptr;
    }
    inline void render_shadows(const RenderData& data) override {
        int i = 0;
        for (auto& [name, texture] : textures) {
            texture->bind(i);
            shader->uniform(name, i);
            i++;
        }
    }

private:
    std::map<T, std::shared_ptr<Mesh>> meshes;
    std::shared_ptr<Shader> shader;
    std::map<std::string, std::shared_ptr<Texture>> textures;
    frustum::Frustum* frustum;
};
