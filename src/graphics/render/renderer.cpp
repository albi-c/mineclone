#include "renderer.hpp"

#define SHADOW_SIZE 4096

void Renderer::init(Camera* camera, int width, int height) {
    this->camera = camera;

    resize(width, height);
    set_sky_color(glm::vec3());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

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

void Renderer::render() {
    glm::mat4 shadow_transform = render_shadows();
    glm::mat4 bias_matrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, shadow_map_fbo);

    frustum::Frustum frustum = camera->frustum();

    glm::mat4 view = camera->view_matrix();
    glm::mat4 projection = camera->proj_matrix();
    
    glm::mat4 transform = projection * view;

    std::lock_guard<std::mutex> lock(mutex);

    for (auto& [_, group] : groups) {
        if (!group.enabled)
            continue;
        
        std::vector<unsigned int> removed_objects;
        for (auto& [id, object] : group.objects) {
            if (auto obj = object.lock()) {
                if (obj->in_frustum(frustum)) {
                    obj->render({
                        glm::translate(transform, obj->translation()),
                        glm::translate(glm::mat4(), obj->translation()),
                        bias_matrix * glm::translate(shadow_transform, obj->translation()),
                        15
                    });
                }
            } else {
                removed_objects.push_back(id);
            }
        }
        for (auto& id : removed_objects) {
            group.objects.erase(id);
        }
    }
}
glm::mat4 Renderer::render_shadows() {
    if (SHADOW_SIZE == 0)
        return glm::mat4();
    
    glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(glm::vec3(50.0f, 120.0f, 2.0f), glm::vec3(100.0f, 50.0f, 100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::ortho(-190.0f, 190.0f, -110.0f, 105.0f, 30.0f, 500.0f);
    
    glm::mat4 transform = projection * view;

    for (auto& [_, group] : groups) {
        if (!group.enabled)
            continue;
        
        std::vector<unsigned int> removed_objects;
        for (auto& [id, object] : group.objects) {
            if (auto obj = object.lock()) {
                obj->render_shadows({
                    glm::translate(transform, obj->translation()),
                    glm::translate(glm::mat4(), obj->translation()),
                    glm::mat4(),
                    0
                });
            } else {
                removed_objects.push_back(id);
            }
        }
        for (auto& id : removed_objects) {
            group.objects.erase(id);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    return transform;
}

void Renderer::resize(int width, int height) {
    this->width = width;
    this->height = height;

    glViewport(0, 0, width, height);
}

void Renderer::set_sky_color(const glm::vec3& color) {
    sky_color = color;
    glClearColor(sky_color.r, sky_color.g, sky_color.b, 1.0f);
}

unsigned int Renderer::add_object(unsigned int group, std::shared_ptr<Renderable> obj) {
    std::lock_guard<std::mutex> lock(mutex);

    RendererGroup& g = groups[group];
    obj->renderer_id_set(g.id_counter);
    g.objects[g.id_counter] = obj;
    return g.id_counter++;
}
void Renderer::remove_object(unsigned int group, std::shared_ptr<Renderable> obj) {
    remove_object(group, obj->renderer_id_get());
}
void Renderer::remove_object(unsigned int group, unsigned int id) {
    std::lock_guard<std::mutex> lock(mutex);

    groups[group].objects.erase(id);
}
std::shared_ptr<Renderable> Renderer::get_object(unsigned int group, unsigned int id) {
    std::lock_guard<std::mutex> lock(mutex);
    
    return groups[group].objects[id].lock();
}

void Renderer::group_enable(unsigned int group, bool enabled) {
    groups[group].enabled = enabled;
}
void Renderer::group_disable(unsigned int group) {
    group_enable(group, false);
}
bool Renderer::group_enabled(unsigned int group) {
    return groups[group].enabled;
}
