#include "renderer.hpp"

void Renderer::init(Camera* camera, int width, int height) {
    this->camera = camera;

    resize(width, height);
    set_sky_color(glm::vec4());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void Renderer::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera->view_matrix();
    glm::mat4 projection = camera->proj_matrix();
    
    glm::mat4 transform = projection * view;

    for (auto& [id, mt] : meshes) {
        Mesh* mesh = mt.first;
        glm::mat4 transform_ = glm::translate(transform, mt.second);
        glm::mat4 model = glm::translate(glm::mat4(), mt.second);
        mesh->shader->uniform("transform", transform_);
        mesh->shader->uniform("model", model);
        mesh->render();
    }
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
