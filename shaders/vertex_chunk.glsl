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
out vec4 fragVertexColor;

void main()
{
    vec3 world = (worldPosition + vec3(0.5)) * particleScale + (aPos * particleScale);
    gl_Position = projection * view * vec4(world, 1.0);

    // Extract camera position from the view matrix
    // vec3 cameraPos = vec3(inverse(view)[3]);

    // Light direction from camera to the fragment
    vec3 lightDir = normalize(vec3(inverse(view)[3]) - world);

    // Simple diffuse shading
    float diff = max(dot(aNormal, lightDir), 0.0);

    // Apply shading to base color
    fragVertexColor = colors[int(aParticleType)] * vec4(vec3(diff), 1.0f);
}
