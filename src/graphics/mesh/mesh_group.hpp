#pragma once

#include <map>
#include <memory>

#include "graphics/mesh/mesh.hpp"
#include "graphics/shaders/shader.hpp"
#include "graphics/textures/texture.hpp"
#include "graphics/render/renderable.hpp"

template <class T>
class MeshGroup : public Renderable {
public:
    inline MeshGroup(
        std::shared_ptr<Shader> shader,
        const std::map<std::string, std::shared_ptr<Texture>>& textures
    )
        : shader(shader), textures(textures) {}

    inline std::shared_ptr<Mesh>& operator[](const T& id) {
        return meshes[id];
    }
    inline void erase(const T& id) {
        meshes.erase(id);
    }
    inline bool contains(const T& id) {
        return meshes.contains(id);
    }

    inline glm::vec3 translation() const override {
        return glm::vec3(0.0f);
    }

    inline bool in_frustum(const frustum::Frustum& frustum) override {
        this->frustum = frustum;
        return true;
    }

    inline bool basic_render() const override {
        return true;
    }

    inline void render(const RenderData& data) override {
        shader->use();

        int i = 0;
        for (auto& [name, texture] : textures) {
            texture->bind(i);
            shader->uniform_b(name, i);
            i++;
        }

        static glm::mat4 bias_matrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );

        shader->uniform_b("ortho", data.ortho);
        shader->uniform_b("shadowMap", data.shadow_map);
        shader->uniform_b("shadowMapEnabled", data.shadow_map_enabled);

        for (auto& [id, mesh] : meshes) {
            if (mesh->in_frustum(frustum)) {
                shader->uniform_b("transform", glm::translate(data.transform, mesh->translation()));
                shader->uniform_b("model", glm::translate(data.model, mesh->translation()));
                shader->uniform_b("shadow_transform", bias_matrix * glm::translate(data.shadow_transform, mesh->translation()));

                mesh->render_basic();
            }
        }
    }
    inline void render_shadows(const RenderData& data) override {
        shader->shadow->use();

        int i = 0;
        for (auto& [name, texture] : textures) {
            texture->bind(i);
            shader->shadow->uniform_b(name, i);
            i++;
        }

        for (auto& [id, mesh] : meshes) {
            shader->shadow->uniform_b("transform", glm::translate(data.transform, mesh->translation()));
            shader->shadow->uniform_b("model", glm::translate(data.model, mesh->translation()));
            mesh->render_basic();
        }
    }

    inline size_t num_meshes() {
        return meshes.size();
    }

private:
    std::map<T, std::shared_ptr<Mesh>> meshes;
    std::shared_ptr<Shader> shader;
    std::map<std::string, std::shared_ptr<Texture>> textures;
    frustum::Frustum frustum;
};
