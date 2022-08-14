#include "shader.hpp"

static const std::map<BuiltinShader, std::pair<std::string, std::string>> BUILTIN_SHADERS = {
{BuiltinShader::BLOCK, {R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTex;
layout (location = 2) in vec3 aNormal;

out vec3 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 ShadowCoord;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 shadow_transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);

    ShadowCoord = shadow_transform * vec4(aPos, 1.0);

    TexCoord = aTex;
    Normal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
}
)", R"(
#version 330 core
in vec3 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 ShadowCoord;

out vec4 FragColor;

uniform sampler2DArray textureArray;
uniform sampler2D shadowMap;

struct light_t {
    vec3 ambient;

    vec3 diffuse;
    vec3 diffuse_pos;

    vec3 sun;
};
uniform light_t light;

float shadow();

void main() {
    vec4 color = texture(textureArray, TexCoord);
    if (color.a == 0.0)
        discard;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.diffuse_pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    vec3 ambient = light.ambient;
    
    vec3 light = (diffuse + ambient) * shadow() * light.sun;

    color *= vec4(light, 1.0);

    color /= vec4(color.rgb + 1.0, 1.0);
    
    FragColor = vec4(pow(color.rgb, vec3(1.0 / 2.2)), 1.0);
}

float shadow() {
    float visibility = 1.0;
    float bias = 0.0003;

    float shadowValue = texture(shadowMap, ShadowCoord.xy).x * 0.5;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            shadowValue += texture(shadowMap, ShadowCoord.xy + vec2(x / 128, y / 128)).x * 0.02;
        }
    }
    if (shadowValue < ShadowCoord.z - bias)
        visibility *= 0.1;
    
    return visibility;
}
)"}},
{BuiltinShader::DEPTH, {R"(
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

uniform sampler2DArray textureArray;

void main() {
    if (texture(textureArray, TexCoord).a == 0.0)
        discard;
}
)"}},
{BuiltinShader::GUI, {R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 Color;

uniform mat4 ortho;

void main() {
    gl_Position = ortho * vec4(aPos, 1.0, 1.0);

    Color = aColor;
}
)", R"(
#version 330 core
in vec3 Color;

out vec4 FragColor;

void main() {
    FragColor = vec4(Color, 1.0);
}
)"}}
};

Shader::Shader(Shader* other)
    : shadow(other->shadow) {
    
    program = other->program;
}
Shader::Shader(const std::string& vertex_code, const std::string& fragment_code) {
    shadow = new Shader(BuiltinShader::DEPTH);
    init(vertex_code, fragment_code);
}
Shader::Shader(BuiltinShader shader) {
    if (shader != BuiltinShader::DEPTH)
        shadow = new Shader(BuiltinShader::DEPTH);
    init(BUILTIN_SHADERS.at(shader).first, BUILTIN_SHADERS.at(shader).second);
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
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
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
