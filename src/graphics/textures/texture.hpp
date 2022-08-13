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
    Texture();
    Texture(Texture* other);
    Texture(const std::string& filename);
    Texture(const unsigned char* data, int width, int height);

    inline virtual void bind(int texture_unit) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

private:
    GLuint texture;
};
