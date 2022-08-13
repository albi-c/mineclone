#pragma once

#include <map>
#include <vector>

#include "lib/opengl.hpp"
#include "graphics/shaders/uniform.hpp"

struct RenderData {
    glm::mat4 transform;
    glm::mat4 model;
    glm::mat4 shadow_transform;

    GLuint shadow_map;
};

class Renderable {
public:
    virtual void render(const RenderData& data) =0;
    virtual void render_shadows(const RenderData& data) =0;

    inline virtual bool is_3d() const {
        return true;
    }

    inline virtual glm::vec3 translation() const {
        return glm::vec3(0.0f);
    }

    inline void renderer_id_set(unsigned int id) { renderer_id = id; }
    inline unsigned int renderer_id_get() const { return renderer_id; }

private:
    unsigned int renderer_id = 0;
};
