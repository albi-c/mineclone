#pragma once

#include <map>
#include <memory>
#include <utility>

#include "lib/opengl.hpp"
#include "graphics/mesh.hpp"
#include "camera.hpp"

class Renderer {
public:
    void init(Camera* camera, int width, int height);

    void render2d(Mesh& mesh);
    void render3d(Mesh& mesh, const glm::vec3& translate = glm::vec3());
    void render();
    glm::mat4 render_shadows();

    void resize(int width, int height);

    void set_sky_color(const glm::vec3& color);

    void add_mesh(int id, Mesh* mesh, const glm::vec3& translate = glm::vec3());
    void remove_mesh(int id);
    Mesh* get_mesh(int id);
    std::map<int, std::pair<Mesh*, glm::vec3>>& get_meshes();

private:
    int width, height;
    glm::vec3 sky_color;

    Camera* camera;

    std::map<int, std::pair<Mesh*, glm::vec3>> meshes;

    GLuint shadow_map_fbo, shadow_map;
};
