#pragma once

#include "texture.hpp"

class TextureArray : public Texture {
public:
    TextureArray();
    TextureArray(TextureArray* other);

    inline void bind(int texture_unit) override {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    }

    void add(const std::string& name, const std::string& filename);

    void generate();

    int position(const std::string& name) const;

private:
    std::map<std::string, std::string> textures;
    std::map<std::string, int> positions;

    GLuint texture;
};
