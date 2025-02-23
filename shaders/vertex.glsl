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

void main()
{
    vec3 world = worldPosition * particleScale + (aPos * particleScale);
    gl_Position = projection * view * vec4(world, 1.0);

    // Extract camera position from the view matrix
    // vec3 cameraPos = vec3(inverse(view)[3]);

    // Light direction from camera to the fragment
    vec3 lightDir = normalize(vec3(inverse(view)[3]) - world);

    // Simple diffuse shading
    float diff = max(dot(aNormal, lightDir), 0.0);

    // Apply shading to base color
    // TODO: Use color passed from CPU
    fragVertexColor = vec3(0.8, 0.5, 0.01) * diff;


}
