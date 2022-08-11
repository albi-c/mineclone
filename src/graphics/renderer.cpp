#include "renderer.hpp"

#define SHADOW_SIZE 4096

const static std::vector<float> HDR_MESH_DATA = {
    -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f
};

void Renderer::init(Camera* camera, int width, int height) {
    
    this->camera = camera;

    resize(width, height);
    set_sky_color(glm::vec4());

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

    glm::mat4 view = camera->view_matrix();
    glm::mat4 projection = camera->proj_matrix();
    
    glm::mat4 transform = projection * view;

    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, shadow_map_fbo);

    for (auto& [id, mt] : meshes) {
        Mesh* mesh = mt.first;
        glm::mat4 transform_ = glm::translate(transform, mt.second);
        glm::mat4 shadow_transform_ = glm::translate(shadow_transform, mt.second);
        glm::mat4 model = glm::translate(glm::mat4(), mt.second);
        mesh->shader->uniform("transform", transform_);
        mesh->shader->uniform("shadow_transform", bias_matrix * shadow_transform_);
        mesh->shader->uniform("model", model);
        mesh->shader->uniform("shadowMap", 15);
        mesh->render();
    }
}
glm::mat4 Renderer::render_shadows() {
    glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 120.0f, 2.0f), glm::vec3(100.0f, 50.0f, 100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::ortho(-190.0f, 190.0f, -110.0f, 105.0f, 30.0f, 500.0f);
    
    glm::mat4 transform = projection * view;

    for (auto& [id, mt] : meshes) {
        Mesh* mesh = mt.first;
        glm::mat4 transform_ = glm::translate(transform, mt.second);
        glm::mat4 model = glm::translate(glm::mat4(), mt.second);
        mesh->shader_shadow.uniform("transform", transform_);
        mesh->shader_shadow.uniform("model", model);
        mesh->shader_shadow.uniform("shadowMap", 5);
        mesh->render_shadows();
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

void Renderer::set_sky_color(const glm::vec4& color) {
    sky_color = color;
    glClearColor(sky_color.r, sky_color.g, sky_color.b, sky_color.a);
}

void Renderer::add_mesh(int id, Mesh* mesh, const glm::vec3& translate) {
    meshes[id] = {mesh, translate};
}
void Renderer::remove_mesh(int id) {
    meshes.erase(id);
}
Mesh* Renderer::get_mesh(int id) {
    return meshes[id].first;
}
std::map<int, std::pair<Mesh*, glm::vec3>>& Renderer::get_meshes() {
    return meshes;
}
