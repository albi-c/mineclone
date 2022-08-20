#version 330 core
in vec3 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 ShadowCoord;

out vec4 FragColor;

uniform sampler2DArray textureArray;
uniform sampler2D shadowMap;
uniform bool shadowMapEnabled;

uniform vec3 camera_pos;

#define SHADOW_RANGE 2
#define SHADOW_COUNT (((SHADOW_RANGE) * 2 + 1) * ((SHADOW_RANGE) * 2 + 1))

uniform struct light_t {
    vec3 ambient;

    vec3 diffuse;
    vec3 diffuse_pos;
    vec3 diffuse_dir;

    vec3 sun;
} light;

float shadow();

void main() {
    vec4 color_a = texture(textureArray, vec3(TexCoord));
    if (color_a.a == 0.0)
        discard;
    
    float fog = clamp((length(FragPos.xz - camera_pos.xz) - 20.0) / 100.0, 0.0, 1.0);
    if (fog > 0.8)
        discard;
    
    vec3 color = color_a.rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.diffuse_pos - FragPos);
    lightDir = normalize(light.diffuse_dir);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    vec3 ambient = light.ambient;
    
    vec3 light = (diffuse + ambient) * shadow() * light.sun;

    color *= light;

    color = mix(color, vec3(1.0), fog);

    color /= color.rgb + 1.0;
    
    FragColor = vec4(pow(color, vec3(1.0 / 1.8)), 1.0);
}

float shadow() {
    if (!shadowMapEnabled)
        return 1.0;

    vec2 texelSize = vec2(1.0) / textureSize(shadowMap, 0).xy * 0.25;
    float visibility = 0.0;
    float bias = 0.0003;

    for (int x = -SHADOW_RANGE; x <= SHADOW_RANGE; x++) {
        for (int y = -SHADOW_RANGE; y <= SHADOW_RANGE; y++) {
            if (texture(shadowMap, ShadowCoord.xy + texelSize * vec2(x, y)).x > ShadowCoord.z - bias)
                visibility += 1.0;
        }
    }
    
    return min(visibility / SHADOW_COUNT + 0.1, 1.0);
}
