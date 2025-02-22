#version 460 core

layout(location = 0) in vec3 aPos;

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
    gl_Position = projection * view * vec4(worldPosition * particleScale + (aPos * particleScale), 1.0);

    fragVertexColor = vec3(0.5) * max(0.0, -dot(     mat3(transpose(view)) * vec3(0, 0, -1)     , vec3(0, 1, 0)));
}
