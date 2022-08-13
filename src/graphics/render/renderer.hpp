#pragma once

#include <map>
#include <memory>
#include <utility>

#include "lib/opengl.hpp"
#include "renderable.hpp"
#include "camera/camera.hpp"

struct RendererGroup {
    std::map<unsigned int, std::weak_ptr<Renderable>> objects;
    unsigned int id_counter;
    bool enabled = true;
};

class Renderer {
public:
    void init(Camera* camera, int width, int height);

    void render();
    glm::mat4 render_shadows();

    void resize(int width, int height);

    void set_sky_color(const glm::vec3& color);

    unsigned int add_object(unsigned int group, std::shared_ptr<Renderable> obj);
    void remove_object(unsigned int group, const std::shared_ptr<Renderable> obj);
    void remove_object(unsigned int group, unsigned int id);
    std::shared_ptr<Renderable> get_object(unsigned int group, unsigned int id);

    void group_enable(unsigned int group, bool enabled = true);
    void group_disable(unsigned int group);
    bool group_enabled(unsigned int group);

private:
    int width, height;
    glm::vec3 sky_color;

    Camera* camera;

    std::map<int, RendererGroup> groups;
    std::mutex mutex;

    GLuint shadow_map_fbo, shadow_map;
};
