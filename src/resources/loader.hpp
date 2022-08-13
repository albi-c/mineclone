#pragma once

#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <memory>

#include "util/file.hpp"
#include "util/string.hpp"
#include "graphics/textures/texture.hpp"
#include "graphics/textures/texture_array.hpp"

class ResourceLoader {
public:
    static std::vector<unsigned char> vector(const std::string& name);
    static std::string string(const std::string& name);

    static std::shared_ptr<Texture> texture(const std::string& name);
    static std::shared_ptr<TextureArray> texture_array(const std::string& name, bool names_to_uppercase = false);
};
