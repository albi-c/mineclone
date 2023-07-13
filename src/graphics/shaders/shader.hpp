#pragma once

#include <map>
#include <utility>
#include <string>
#include <memory>

#include "opengl.hpp"

enum class BuiltinShader {
    BLOCK, DEPTH, GUI
};

class Shader {
public:
    std::shared_ptr<Shader> shadow;

    Shader(Shader* other);
    Shader(const std::string& vertex_code, const std::string& fragment_code, bool load_shadow = true);

    void use();

    void uniform(const std::string& name, int value);
    void uniform(const std::string& name, GLuint value);
    void uniform(const std::string& name, float value);
    void uniform(const std::string& name, double value);

    void uniform(const std::string& name, const glm::vec2& value);
    void uniform(const std::string& name, const glm::vec3& value);
    void uniform(const std::string& name, const glm::vec4& value);

    void uniform(const std::string& name, const glm::mat4& value);

    void uniform_b(const std::string& name, int value);
    void uniform_b(const std::string& name, GLuint value);
    void uniform_b(const std::string& name, float value);
    void uniform_b(const std::string& name, double value);

    void uniform_b(const std::string& name, const glm::vec2& value);
    void uniform_b(const std::string& name, const glm::vec3& value);
    void uniform_b(const std::string& name, const glm::vec4& value);

    void uniform_b(const std::string& name, const glm::mat4& value);

private:
    GLuint program;

    void init(const std::string& vertex_code, const std::string& fragment_code);

    void prepare_set_uniform();
};
