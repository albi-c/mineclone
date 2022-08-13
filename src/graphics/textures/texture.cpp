#include "texture.hpp"

static unsigned char* load_texture_data(const std::string& filename, int& width, int& height, int& n_channels) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &n_channels, TEXTURE_PIXEL_SIZE);
    if (!data) {
        raiseError("Couldn't load texture | " + filename);
    }

    return data;
}

static void free_texture_data(unsigned char* data) {
    stbi_image_free(data);
}

static GLuint load_texture(const unsigned char* data, int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

static GLuint load_texture(const std::string& filename) {
    int width, height, n_channels;
    unsigned char* data = load_texture_data(filename, width, height, n_channels);

    GLuint texture = load_texture(data, width, height);

    free_texture_data(data);

    return texture;
}

Texture::Texture() {}
Texture::Texture(Texture* other) {
    texture = other->texture;
}
Texture::Texture(const std::string& filename) {
    texture = load_texture(filename);
}

Texture::Texture(const unsigned char* data, int width, int height) {
    texture = load_texture(data, width, height);
}
