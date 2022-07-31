#pragma once

#include <map>
#include <memory>

#include "opengl.hpp"
#include "mesh.hpp"
#include "../camera.hpp"

class Renderer {
public:
    void init(Camera* camera, int width, int height);

    void render();

    void resize(int width, int height);

    void set_sky_color(const glm::vec4& color);

    void add_mesh(int id, Mesh* mesh);
    void remove_mesh(int id);
    Mesh* get_mesh(int id);
    std::map<int, Mesh*>& get_meshes();

private:
    int width, height;
    glm::vec4 sky_color;

    Camera* camera;

    std::map<int, Mesh*> meshes;
};
