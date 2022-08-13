#pragma once

#include <map>
#include <vector>

#include "lib/opengl.hpp"
#include "graphics/shaders/uniform.hpp"

class Renderable {
public:
    virtual void render(const std::map<std::string, Uniform>& uniforms) =0;
    virtual void render_shadows(const std::map<std::string, Uniform>& uniforms) =0;

    virtual std::vector<std::string> required_textures() const =0;

    inline virtual glm::vec3 translation() const {
        return glm::vec3(0.0f);
    }

    inline void renderer_id_set(unsigned int id) { renderer_id = id; }
    inline unsigned int renderer_id_get() const { return renderer_id; }

private:
    unsigned int renderer_id = 0;
};
