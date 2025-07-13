#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

layout (binding = 3, std430) readonly restrict buffer Colors {
    vec4 colors[];
};

uniform uint particleType;
uniform float particleScale;
uniform vec3 position;
uniform mat4 view;
uniform mat4 projection;

out vec4 fragVertexColor;

void main() {
    vec3 world = (position + vec3(0.5)) * particleScale + (aPos * particleScale);
    gl_Position = projection * view * vec4(world, 1.0);
    fragVertexColor = colors[particleType];
    fragVertexColor.a = 0.1;
}
