#pragma once

#include <map>
#include <memory>
#include <utility>

#include "lib/opengl.hpp"
#include "renderable.hpp"
#include "camera/camera.hpp"

struct RendererGroupOptions {
    bool enabled = true;
    bool shadows_enabled = true;
    bool cull_faces = true;
};

struct RendererGroup {
    std::map<unsigned int, std::weak_ptr<Renderable>> objects;
    unsigned int id_counter;
    RendererGroupOptions options;
};

struct RenderFlags {
    bool render_shadows = true;
    bool cull_faces = true;
};

class Renderer {
public:
    static void init(int width, int height);

    static void render_start();
    static void render(std::shared_ptr<Renderable> obj, const RenderFlags& flags = RenderFlags{});
    static void render_end();

    static void resize(int width, int height);

    static void set_sky_color(const glm::vec3& color);

private:
    static inline int width, height;
    static inline glm::vec3 sky_color;

    static inline std::shared_ptr<Camera> camera;

    static inline std::vector<std::pair<std::weak_ptr<Renderable>, RenderFlags>> render_queue;
    static inline std::mutex mutex;

    static inline GLuint shadow_map_fbo, shadow_map;

    static glm::mat4 render_shadows();
};
