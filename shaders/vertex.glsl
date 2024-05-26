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
uniform float ParticleScale;
uniform vec3 CubeTranslation;

/* Out Variables */
out vec3 fragVertexColor;
out vec3 fragOrigin;
out vec3 fragDirection;

void main()
{
    vec3 translation = vec3(0.0);
    // I want to be able to scale the cube smh
    // Add 0.5 to aPos so that the bottom left frontmost corner of the cube is at (0,0)
    // Scale the cube by the ParticleScale so it is the size of a single voxel
    // Scale again by the size of the array (50.0) to make it that many voxels big
    //   Add 1.0 to the array size becasue of offset stuff I guess
    //     (actually I think the platform size is 1 bigger than it should be)
    vec3 aPosScaled = ((aPos + 0.5) * ParticleScale * 50.0) + translation;
    // vec3 aPosScaled = (scale * vec4(aPos, 1.0)).xyz; // (aPos + vec3(0.5)) * (50.0 + 1.0);

    // Transform vertex position from local-space to clip-space
    gl_Position = projection * view * model * vec4(aPosScaled, 1.0);

    // Convert camera position from world-space to local-space of the model
    vec3 cameraLocal = (inverse(model) * vec4(CameraPosition, 1.0)).xyz;

    // Ray origin and direction
    //  translation (in normal) - 0.5 vv                   vv array size
    // fragOrigin = (cameraLocal - (-translate * scale * vec4(0.5)).xyz) / ParticleScale;
    fragOrigin = (cameraLocal - translation) / ParticleScale;
    fragDirection = (aPosScaled - cameraLocal);

    // Providing color to fragment shader
    fragVertexColor = vec3(1.0);
}
