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

struct block_properties_t {
    int plant;
    int waving;

    int b[32];
};
uniform block_properties_t block_properties;

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu;
    const uint ieeeOne      = 0x3F800000u;

    m &= ieeeMantissa;
    m |= ieeeOne;

    float  f = uintBitsToFloat( m );
    return f - 1.0;
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

void main() {
    vec3 pos = aPos;

    FragPos = vec3(model * vec4(pos, 1.0));

    if ((block_properties.b[int(aBlock)] & block_properties.waving) != 0)
        if ((block_properties.b[int(aBlock)] & block_properties.plant) != 0)
            pos += vec3(sin(time + random(random(FragPos.z)) * time), 0.0, cos(time + random(random(FragPos.z)) * time)) * 0.1 * aTex.y;
        else
            pos += vec3(sin(time + random(random(FragPos.z)) * time), 0.0, cos(time + random(random(FragPos.z)) * time)) * 0.04;

    gl_Position = transform * vec4(pos, 1.0);

    ShadowCoord = shadow_transform * vec4(pos, 1.0);

    TexCoord = aTex;
    Normal = aNormal;
}
