#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in float aBlock;

out vec3 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 ShadowCoord;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 shadow_transform;

uniform float time;

struct block_ids_t {
    float leaves;
    float grass;
};
uniform block_ids_t block_ids;
struct block_properties_t {
    int plant;
    int waving;

    int b[32];
};
uniform block_properties_t block_properties;

void main() {
    vec3 pos = aPos;

    FragPos = vec3(model * vec4(pos, 1.0));

    if ((block_properties.b[int(aBlock)] & block_properties.waving) != 0)
        if ((block_properties.b[int(aBlock)] & block_properties.plant) != 0)
            pos += vec3(sin(time + FragPos.z), 0.0, cos(time + FragPos.x)) * 0.1 * aTex.y;
        else
            pos += vec3(cos(time + FragPos.z), 0.0, sin(time + FragPos.x)) * 0.05;

    gl_Position = transform * vec4(pos, 1.0);

    ShadowCoord = shadow_transform * vec4(pos, 1.0);

    TexCoord = aTex;
    Normal = aNormal;
}
