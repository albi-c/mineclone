#include "graphics/shaders/shader.hpp"

#include "resources/loader.hpp"

Shader::Shader(Shader* other) {
    shadow = other->shadow;
    program = other->program;
}
Shader::Shader(const std::string& vertex_code, const std::string& fragment_code, bool load_shadow) {
    if (load_shadow)
        shadow = ResourceLoader::shader("shadow");
    init(vertex_code, fragment_code);
}

void Shader::use() {
    glUseProgram(program);
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
    glUniform1f(glGetUniformLocation(program, name.c_str()), (float)value);
}

void Shader::uniform(const std::string& name, const glm::vec2& value) {
    prepare_set_uniform();
    glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}
void Shader::uniform(const std::string& name, const glm::vec3& value) {
    prepare_set_uniform();
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}
void Shader::uniform(const std::string& name, const glm::vec4& value) {
    prepare_set_uniform();
    glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}

void Shader::uniform(const std::string& name, const glm::mat4& value) {
    prepare_set_uniform();
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::uniform_b(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::uniform_b(const std::string& name, GLuint value) {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::uniform_b(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::uniform_b(const std::string& name, double value) {
    glUniform1f(glGetUniformLocation(program, name.c_str()), (float)value);
}

void Shader::uniform_b(const std::string& name, const glm::vec2& value) {
    glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}
void Shader::uniform_b(const std::string& name, const glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}
void Shader::uniform_b(const std::string& name, const glm::vec4& value) {
    glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}

void Shader::uniform_b(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::init(const std::string& vertex_code, const std::string& fragment_code) {
    GLuint vertex_shader, fragment_shader;
    const char* vertex_code_c = vertex_code.c_str();
    const char* fragment_code_c = fragment_code.c_str();
    int success;
    char infoLog[2048];

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_code_c, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 2048, NULL, infoLog);
        raiseError("Couldn't compile vertex shader | " + std::string(infoLog));
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_code_c, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 2048, NULL, infoLog);
        raiseError("Couldn't compile fragment shader | " + std::string(infoLog));
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 2048, NULL, infoLog);
        raiseError("Couldn't link shaders | " + std::string(infoLog));
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::prepare_set_uniform() {
    glUseProgram(program);
}
