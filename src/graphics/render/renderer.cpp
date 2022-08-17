#include "renderer.hpp"

#define SHADOW_SIZE 4096

void RendererHandlers::framebuffer_resize_event_handler(const EventFramebufferResize& e) {
    Renderer::resize(e.width, e.height);
}

void Renderer::init(int width, int height) {
    resize(width, height);
    set_sky_color(glm::vec3());

    EventManager::listen(handlers.framebuffer_resize_event_queue);

    glEnable(GL_DEPTH_TEST);

    // SHADOW MAP
    glGenFramebuffers(1, &shadow_map_fbo);

    glGenTextures(1, &shadow_map);
    glBindTexture(GL_TEXTURE_2D, shadow_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = {
        1.0f, 1.0f, 1.0f, 1.0f
    };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render_start() {
    handlers.framebuffer_resize_event_queue.process();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Renderer::render(std::shared_ptr<Renderable> obj, const RenderOptions& flags) {
    std::lock_guard<std::mutex> lock(mutex);

    render_queue.push_back({obj, flags});
}
void Renderer::render_end() {
    std::lock_guard<std::mutex> lock(mutex);
    
    glm::mat4 shadow_transform = render_shadows();
    glm::mat4 bias_matrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );

    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, shadow_map_fbo);

    frustum::Frustum frustum = Camera::frustum();

    glm::mat4 view = Camera::view_matrix();
    glm::mat4 projection = Camera::proj_matrix();
    
    glm::mat4 transform = projection * view;

    glm::mat4 ortho = Camera::ortho_matrix();

    bool shadows = shadows_enabled();

    for (auto& [object, flags] : render_queue) {
        if (flags.cull_faces)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        
        if (auto obj = object.lock()) {
            if (obj->in_frustum(frustum)) {
                obj->render({
                    glm::translate(transform, obj->translation()),
                    glm::translate(glm::mat4(), obj->translation()),
                    bias_matrix * glm::translate(shadow_transform, obj->translation()),
                    ortho,
                    15,
                    shadows
                });
            }
        }
    }

    render_queue.clear();
}

void Renderer::resize(int width, int height) {
    Renderer::width = width;
    Renderer::height = height;

    glViewport(0, 0, width, height);
}

void Renderer::set_sky_color(const glm::vec3& color) {
    sky_color = color;
    glClearColor(sky_color.r, sky_color.g, sky_color.b, 1.0f);
}

bool Renderer::shadows_enabled() {
    return (SHADOW_SIZE != 0) && Options::get("shadows");
}

glm::mat4 Renderer::render_shadows() {
    if (!shadows_enabled())
        return glm::mat4();
    
    glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(
        glm::vec3(50.0f, 120.0f, 2.0f) + glm::vec3(Camera::pos.x, 0.0f, Camera::pos.z),
        glm::vec3(100.0f, 50.0f, 100.0f) + glm::vec3(Camera::pos.x, 0.0f, Camera::pos.z),
        glm::vec3(0.0f, 1.0f, 0.0f)
        );
    glm::mat4 projection = glm::ortho(-190.0f, 190.0f, -110.0f, 105.0f, 0.0f, 500.0f);
    
    glm::mat4 transform = projection * view;

    glm::mat4 ortho = camera->ortho_matrix();

    glEnable(GL_CULL_FACE);

    for (auto& [object, flags] : render_queue) {
        if (!flags.render_shadows)
            continue;
        
        if (auto obj = object.lock()) {
            obj->render_shadows({
                glm::translate(transform, obj->translation()),
                glm::translate(glm::mat4(), obj->translation()),
                glm::mat4(),
                ortho,
                0,
                false
            });
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    return transform;
}
