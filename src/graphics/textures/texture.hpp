#pragma once

#define TEXTURE_PIXEL_SIZE 4

#include <map>
#include <vector>
#include <utility>
#include <cstring>

#include "lib/opengl.hpp"
#include "lib/stb_image.h"

class Texture {
public:
    GLuint texture;

    Texture(const std::string& filename);
    Texture(const unsigned char* data, int width, int height);
};
