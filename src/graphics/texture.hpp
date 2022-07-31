#pragma once

#define TEXTURE_PIXEL_SIZE 4

#include <map>
#include <vector>
#include <utility>
#include <cstring>


#include "opengl.hpp"
#include "../util.hpp"
#include "../lib/stb_image.h"

class Texture {
public:
    GLuint texture;

    Texture(const std::string& filename);
    Texture(const unsigned char* data, int width, int height);
};

class TextureAtlas {
public:
    void add(const std::string& name, const std::string& filename);

    Texture* generate();

    glm::vec2 position(const std::string& name);
    float size();

    glm::vec2 transform(const std::string& name, glm::vec2 pos);
    void transform(const std::string& name, float& x, float& y);
    void transform(const std::string& name, float x, float y, float& xo, float& yo);

private:
    std::map<std::string, std::string> textures;
    std::map<std::string, glm::vec2> positions;

    int n_tex, tsize;
};

class Texture3D {
public:
    GLuint texture;

    void add(const std::string& name, const std::string& filename);

    void generate();

    int position(const std::string& name);

private:
    std::map<std::string, std::string> textures;
    std::map<std::string, int> positions;
};
