#pragma once

#include <map>
#include <utility>
#include <string>

#include "../util.hpp"
#include "opengl.hpp"

enum class BuiltinShader {
    BLOCK, DEPTH, POST
};

class Shader {
public:
    Shader(const std::string& vertex_code, const std::string& fragment_code);
    Shader(BuiltinShader shader);

    void use();

    void uniform(const std::string& name, bool value);
    void uniform(const std::string& name, int value);
    void uniform(const std::string& name, GLuint value);
    void uniform(const std::string& name, float value);
    void uniform(const std::string& name, double value);

    void uniform(const std::string& name, glm::vec2 value);
    void uniform(const std::string& name, glm::vec3 value);
    void uniform(const std::string& name, glm::vec4 value);

    void uniform(const std::string& name, glm::mat4 value);

private:
    GLuint program;

    void init(const std::string& vertex_code, const std::string& fragment_code);

    void prepare_set_uniform();
};
