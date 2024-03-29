#include "resources/loader.hpp"

std::vector<unsigned char> ResourceLoader::vector(const std::string& name) {
    std::ifstream input("res/" + name);
    if (!input.good()) {
        throw std::runtime_error("file not found: " + name);
    }
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {});
}
std::string ResourceLoader::string(const std::string& name) {
    auto v = ResourceLoader::vector(name);
    return std::string((char*)&v[0], v.size());
}

std::shared_ptr<Texture> ResourceLoader::texture(const std::string& name) {
    return std::make_shared<Texture>("res/textures/" + name + ".png");
}
std::shared_ptr<TextureArray> ResourceLoader::texture_array(const std::string& name, bool names_to_uppercase) {
    std::ifstream am("res/textures/" + name + "/texture_array.txt");
    auto tex_arr = std::make_shared<TextureArray>();
    std::string line;
    while (am >> line) {
        if (!line.empty()) {
            tex_arr->add(names_to_uppercase ? su::to_upper(line) : line, "res/textures/" + name + "/" + line + ".png");
        }
    }
    tex_arr->generate();
    return tex_arr;
}

std::shared_ptr<Shader> ResourceLoader::shader(const std::string& name) {
    return std::make_shared<Shader>(string("shaders/" + name + ".vert"), string("shaders/" + name + ".frag"), name != "shadow");
}
