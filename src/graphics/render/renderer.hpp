#pragma once

#include <map>
#include <memory>
#include <utility>

#include "lib/opengl.hpp"
#include "renderable.hpp"
#include "camera/camera.hpp"

class Renderer {
public:
    void init(Camera* camera, int width, int height);

    void render();
    glm::mat4 render_shadows();

    void resize(int width, int height);

    void set_sky_color(const glm::vec3& color);

    unsigned int add_object(std::shared_ptr<Renderable> obj);
    void remove_object(const std::shared_ptr<Renderable> obj);
    void remove_object(unsigned int id);
    std::shared_ptr<Renderable> get_object(unsigned int id);

private:
    int width, height;
    glm::vec3 sky_color;

    Camera* camera;

    std::map<unsigned int, std::weak_ptr<Renderable>> objects;
    unsigned int n_objects = 0;
    std::mutex mutex;

    GLuint shadow_map_fbo, shadow_map;
};
