#version 330 core
in vec3 TexCoord;

uniform sampler2DArray textureArray;

void main() {
    if (texture(textureArray, TexCoord).a == 0.0)
        discard;
}
