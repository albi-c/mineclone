#include "shader.hpp"

static const std::map<BuiltinShader, std::pair<std::string, std::string>> BUILTIN_SHADERS = {
{BuiltinShader::BLOCK, {R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTex;

out vec3 TexCoord;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);

    TexCoord = aTex;
}
)", R"(
#version 330 core
in vec3 TexCoord;

out vec4 FragColor;

uniform sampler2DArray texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}
)"}}
};

Shader::Shader(const std::string& vertex_code, const std::string& fragment_code) {
    init(vertex_code, fragment_code);
}
Shader::Shader(BuiltinShader shader) {
    init(BUILTIN_SHADERS.at(shader).first, BUILTIN_SHADERS.at(shader).second);
}

void Shader::use() {
    glUseProgram(program);
}

void Shader::uniform(const std::string& name, bool value) {
    prepare_set_uniform();
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::uniform(const std::string& name, int value) {
    prepare_set_uniform();
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::uniform(const std::string& name, GLuint value) {
    prepare_set_uniform();
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::uniform(const std::string& name, float value) {
    prepare_set_uniform();
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::uniform(const std::string& name, double value) {
    prepare_set_uniform();
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::uniform(const std::string& name, glm::vec2 value) {
    prepare_set_uniform();
    glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}
void Shader::uniform(const std::string& name, glm::vec3 value) {
    prepare_set_uniform();
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}
void Shader::uniform(const std::string& name, glm::vec4 value) {
    prepare_set_uniform();
    glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}

void Shader::uniform(const std::string& name, glm::mat4 value) {
    prepare_set_uniform();
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::init(const std::string& vertex_code, const std::string& fragment_code) {
    GLuint vertex_shader, fragment_shader;
    const char* vertex_code_c = vertex_code.c_str();
    const char* fragment_code_c = fragment_code.c_str();
    int success;
    char infoLog[512];

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_code_c, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        raiseError("Couldn't compile vertex shader | " + std::string(infoLog));
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_code_c, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        raiseError("Couldn't compile fragment shader | " + std::string(infoLog));
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        raiseError("Couldn't link shaders | " + std::string(infoLog));
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::prepare_set_uniform() {
    glUseProgram(program);
}
