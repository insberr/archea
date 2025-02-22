#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

/* Camera Uniforms */
uniform mat4 view;
uniform mat4 projection;
uniform float particleScale;
uniform vec3 worldPosition;

/* Out Variables */
out vec3 fragVertexColor;
out vec3 fragOrigin;
out vec3 fragDirection;

void main()
{
    vec3 world = worldPosition * particleScale + (aPos * particleScale);
    gl_Position = projection * view * vec4(world, 1.0);

    fragVertexColor = vec3(0.8, 0.5, 0.01) * max(0.0, dot(normalize(-vec3(view[3]) - world), aNormal));
}
