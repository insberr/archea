#version 460 core

/*
Credits to https://github.com/kallisto56/Raymarching-inside-cube for the code
*/

layout(location = 0) in vec3 aPos;

/* Camera Uniforms */
uniform vec3 CameraPosition;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

/* Out Variables */
out vec3 fragVertexColor;
out vec3 fragOrigin;
out vec3 fragDirection;

void main()
{
    // I want to be able to scale the cube smh
    vec3 aPosScaled = aPos * 2.0;

    // Transform vertex position from local-space to clip-space
    gl_Position = projection * view * model * vec4(aPosScaled, 1.0);

    // Convert camera position from world-space to local-space of the model
    vec3 cameraLocal = (inverse(model) * vec4(CameraPosition, 1.0)).xyz;

    // Ray origin and direction
    fragOrigin = cameraLocal;
    fragDirection = (aPosScaled - cameraLocal);

    // Providing color to fragment shader
    fragVertexColor = vec3(1.0);
}
