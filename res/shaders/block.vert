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
