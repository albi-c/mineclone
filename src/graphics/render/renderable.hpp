#pragma once

#include <map>
#include <vector>

#include "lib/opengl.hpp"
#include "camera/frustum.hpp"
#include "graphics/shaders/uniform.hpp"

struct RenderData {
    glm::mat4 transform;
    glm::mat4 model;
    glm::mat4 shadow_transform;
    glm::mat4 ortho;

    GLuint shadow_map;
    bool shadow_map_enabled;
};

class Renderable {
public:
    virtual void render(const RenderData& data) =0;
    virtual void render_shadows(const RenderData& data) =0;

    inline virtual glm::vec3 translation() const {
        return glm::vec3(0.0f);
    }

    inline virtual bool in_frustum(const frustum::Frustum& frustum) const {
        return true;
    }
};
