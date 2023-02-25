#include "texture_array.hpp"

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

TextureArray::TextureArray() {}

void TextureArray::add(const std::string& name, const std::string& filename) {
    textures[name] = filename;
}

void TextureArray::generate() {
    int width, height, n_channels, tsize = -1, n_textures = 0;

    std::map<std::string, unsigned char*> datas;

    for (auto& [name, filename] : textures) {
        unsigned char* data = load_texture_data(filename, width, height, n_channels);

        if (width != height) {
            raiseError("Textures in array must be square");
        }

        if (tsize == -1) {
            tsize = width;
        }
        
        if (tsize != width) {
            raiseError("Textures in array must be the same size");
        }

        datas[name] = data;
        positions[name] = n_textures;
        n_textures++;
    }

    unsigned char* idata = new unsigned char[n_textures * width * height * TEXTURE_PIXEL_SIZE];

    int i = 0;
    for (auto& [name, data] : datas) {
        std::copy(data, data + (width * height * TEXTURE_PIXEL_SIZE), idata + (width * height * TEXTURE_PIXEL_SIZE * i));
        free_texture_data(data);
        i++;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB8_ALPHA8, width, height, n_textures, 0, GL_RGBA, GL_UNSIGNED_BYTE, idata);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

int TextureArray::position(const std::string& name) const {
    return positions.at(name);
}
