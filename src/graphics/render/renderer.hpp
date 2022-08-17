#pragma once

#include <map>
#include <memory>
#include <utility>

#include "lib/opengl.hpp"
#include "renderable.hpp"
#include "camera/camera.hpp"
#include "window/event.hpp"
#include "game/event.hpp"
#include "game/options.hpp"

struct RenderOptions {
    bool render_shadows = true;
    bool cull_faces = true;
};

class Renderer;
struct RendererHandlers {
    static void framebuffer_resize_event_handler(const EventFramebufferResize& e);

    FunctionEventQueue<EventFramebufferResize> framebuffer_resize_event_queue = \
        FunctionEventQueue<EventFramebufferResize>(RendererHandlers::framebuffer_resize_event_handler);
};

class Renderer {
public:
    static void init(int width, int height);

    static void render_start();
    static void render(std::shared_ptr<Renderable> obj, const RenderOptions& flags = RenderOptions{});
    static void render_end();

    static void resize(int width, int height);

    static void set_sky_color(const glm::vec3& color);

private:
    static inline int width, height;
    static inline glm::vec3 sky_color;

    static inline std::shared_ptr<Camera> camera;

    static inline std::vector<std::pair<std::weak_ptr<Renderable>, RenderOptions>> render_queue;
    static inline std::mutex mutex;

    static inline GLuint shadow_map_fbo, shadow_map;

    friend struct RendererHandlers;
    static inline RendererHandlers handlers;

    static bool shadows_enabled();

    static glm::mat4 render_shadows();
};
