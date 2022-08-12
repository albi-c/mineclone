#pragma once

#define TEXTURE_PIXEL_SIZE 4

#include <map>
#include <vector>
#include <utility>
#include <cstring>

#include "lib/opengl.hpp"
#include "lib/stb_image.h"

class TextureArray {
public:
    GLuint texture;

    void add(const std::string& name, const std::string& filename);

    void generate();

    int position(const std::string& name) const;

private:
    std::map<std::string, std::string> textures;
    std::map<std::string, int> positions;
};
