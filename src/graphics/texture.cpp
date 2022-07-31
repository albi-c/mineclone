#include "texture.hpp"

static const std::vector<std::pair<int, int>> atlas_sizes = {
    {1, 1},
    {4, 2},
    {16, 4},
    {64, 8},
    {256, 16},
    {1024, 32}
};

static std::map<std::string, GLuint> texture_cache;

static unsigned char* load_texture_data(const std::string& filename, int& width, int& height, int& n_channels) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &n_channels, TEXTURE_PIXEL_SIZE);
    if (!data) {
        raiseError("Couldn't load texture | " + filename);
    }

    return data;
}

void free_texture_data(unsigned char* data) {
    stbi_image_free(data);
}

static GLuint load_texture(const unsigned char* data, int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

static GLuint load_texture(const std::string& filename) {
    int width, height, n_channels;
    unsigned char* data = load_texture_data(filename, width, height, n_channels);

    GLuint texture = load_texture(data, width, height);

    free_texture_data(data);

    texture_cache[filename] = texture;

    return texture;
}

Texture::Texture(const std::string& filename) {
    if (texture_cache.find(filename) != texture_cache.end()) {
        texture = texture_cache[filename];
    } else {
        texture = load_texture(filename);
    }
}

Texture::Texture(const unsigned char* data, int width, int height) {
    texture = load_texture(data, width, height);
}

void TextureAtlas::add(const std::string& name, const std::string& filename) {
    textures[name] = filename;
}

Texture* TextureAtlas::generate() {
    int width, height, n_channels, ntsize, n_textures = 0;
    tsize = -1;
    std::map<std::string, unsigned char*> datas;

    for (auto& [name, filename] : textures) {
        unsigned char* data = load_texture_data(filename, width, height, n_channels);

        if (width != height) {
            raiseError("Textures in atlas must be square");
        }

        if (tsize == -1) {
            tsize = width;
        }
        
        if (tsize != tsize) {
            raiseError("Textures in atlas must be the same size");
        }

        datas[name] = data;
        n_textures++;
    }

    n_tex = -1;
    for (auto& [num, size] : atlas_sizes) {
        if (n_textures < num) {
            n_tex = size;
            break;
        }
    }
    if (n_tex == -1) {
        raiseError("Too many textures in atlas");
    }

    unsigned long data_size = n_tex * n_tex * tsize * tsize * TEXTURE_PIXEL_SIZE;
    unsigned char* data = new unsigned char[data_size];

    int i = 0, x = 0, y = 0;
    for (auto& [name, d] : datas) {
        for (int row = 0; row < tsize; row++) {
            memcpy(data + (x + y * n_tex * tsize + row * n_tex) * TEXTURE_PIXEL_SIZE * tsize,
                    d + row * tsize * TEXTURE_PIXEL_SIZE, tsize * TEXTURE_PIXEL_SIZE);
        }

        positions[name] = glm::vec2(x, y) / (float) n_tex;

        i++;
        x++;

        if (x > n_tex) {
            x = 0;
            y++;
        }
    }

    Texture* tex = new Texture(data, tsize * n_tex, tsize * n_tex);

    delete[] data;

    return tex;
}

glm::vec2 TextureAtlas::position(const std::string& name) {
    return positions[name];
}
float TextureAtlas::size() {
    return 1.0f / (float) n_tex;
}

glm::vec2 TextureAtlas::transform(const std::string& name, glm::vec2 pos) {
    transform(name, pos.x, pos.y);
    return pos;
}
void TextureAtlas::transform(const std::string& name, float& x, float& y) {
    glm::vec2 pos = position(name);
    float size = this->size();

    x = pos.x + (size * x) + (((x > 0.5) ? size / -2.0f : size / 2.0f) / n_tex);
    y = pos.y + (size * y) + (((y > 0.5) ? size / -2.0f : size / 2.0f) / n_tex);
}
void TextureAtlas::transform(const std::string& name, float x, float y, float& xo, float& yo) {
    transform(name, x, y);
    xo = x;
    yo = y;
}

void Texture3D::add(const std::string& name, const std::string& filename) {
    textures[name] = filename;
}

void Texture3D::generate() {
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
        
        if (tsize != tsize) {
            raiseError("Textures in array must be the same size");
        }

        datas[name] = data;
        positions[name] = n_textures;
        n_textures++;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, width, height, n_textures);

    int i = 0;
    for (auto& [name, data] : datas) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        free_texture_data(data);
        i++;
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

int Texture3D::position(const std::string& name) {
    return positions[name];
}
