#include "loader.hpp"

std::vector<unsigned char> ResourceLoader::vector(const std::string& name) {
    std::ifstream input("res/" + name);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {});
}
std::string ResourceLoader::string(const std::string& name) {
    auto v = ResourceLoader::vector(name);
    return std::string((char*)&v[0], v.size());
}

std::shared_ptr<Texture> ResourceLoader::texture(const std::string& name) {
    return std::make_shared<Texture>(new Texture("res/" + name + ".png"));
}
std::shared_ptr<TextureArray> ResourceLoader::texture_array(const std::string& name, bool names_to_uppercase) {
    std::ifstream am("res/" + name + "/texture_array.txt");
    auto tex_arr = std::make_shared<TextureArray>(new TextureArray());
    std::string line;
    while (am >> line) {
        if (!line.empty()) {
            tex_arr->add(names_to_uppercase ? su::to_upper(line) : line, "res/" + name + "/" + line + ".png");
        }
    }
    return tex_arr;
}
