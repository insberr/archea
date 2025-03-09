#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in float aParticleType;

layout (binding = 3, std430) readonly restrict buffer Colors {
    vec4 colors[];
};

/* Camera Uniforms */
uniform mat4 view;
uniform mat4 projection;
uniform float particleScale;
uniform vec3 worldPosition;

/* Out Variables */
out vec3 FragPos;
out vec3 Normal;
out vec4 Color;

void main()
{
    FragPos = worldPosition + aPos;
    Normal = aNormal;
    Color = colors[int(aParticleType)];

    gl_Position = projection * view * vec4(worldPosition + aPos, 1.0);
}
