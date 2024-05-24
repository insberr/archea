#version 460 core

// Code copied and modified from https://www.shadertoy.com/view/4dX3zl
// -- The raycasting code is somewhat based around a 2D raycasting toutorial found here:
// -- http://lodev.org/cgtutor/raycasting.html

out vec4 FragColor;
in vec3 fragCoord;

layout (binding = 0, std430) readonly restrict buffer Particles {
    int particles[];
};
layout (binding = 1, std430) readonly restrict buffer Colors {
    vec4 colors[];
};

uniform vec2 Resolution;
uniform float Time;
/* Camera Uniforms */
uniform float FieldOfView;
uniform vec3 CameraPosition;
uniform mat4 CameraView;
uniform float ParticleScale;
uniform uint EnableOutlines;
// The number of ray steps to make
uniform int MAX_RAY_STEPS;

// 2D Rotation Function
mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

vec2 rotate2d(vec2 v, float a) {
    float sinA = sin(a);
    float cosA = cos(a);
    return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);
}

// Function to calculate distance from a point to a sphere
float sphereSDF(vec3 point, vec3 center, float radius) {
    return length(point - center) - radius;
}

float cubeSDF(vec3 point, vec3 size) {
    vec3 q = abs(point) - size;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

const vec4 NoParticle = vec4(-1.0);
const int arraySize = 50; // make this a uniform
// Test if a voxel exists here
vec4 getParticle(ivec3 c) {
    // Ground
    if (c.y == -1 && c.x >= 0 && c.z >= 0   && c.z <= arraySize && c.x <= arraySize) return vec4(vec3(0.3), 0.6);

    // Axis Voxels
    if (c.y == -1 && c.z == -1   && c.x >= -1) return vec4(1.0, 0.0, 0.0, 0.4); // x axis
    if (c.x == -1 && c.z == -1   && c.y >= -1) return vec4(0.0, 1.0, 0.0, 0.4); // y axis
    if (c.x == -1 && c.y == -1   && c.z >= -1) return vec4(0.0, 0.0, 1.0, 0.4); // z axis

    // Array index range checks
    if (c.x >= arraySize) return NoParticle;
    if (c.y >= arraySize) return NoParticle;
    if (c.z >= arraySize) return NoParticle;
    if (c.x < 0) return NoParticle;
    if (c.y < 0) return NoParticle;
    if (c.z < 0) return NoParticle;

    // Get the value in the array
    // z * (ysize * xsize) + y * (xsize) + x
    int val = particles[c.z * (arraySize * arraySize) + c.y * (arraySize) + c.x];

    if (val == 0) return NoParticle;

    return colors[val - 1];
}

// Check of we are near the edge of the voxel,
//  and if so return a float to multiply the color value by
// basically if the value is 1.0, its not the edge
float edgeCheck(vec3 rayPos, vec3 rayDir, ivec3 mapPos) {
    vec3 ri = 1.0 / rayDir;
    vec3 rs = sign(rayDir);
    vec3 mini = (mapPos - rayPos + 0.5 - 0.5 * vec3(rs)) * ri;
    float t = max(mini.x, max(mini.y, mini.z));
    vec3 pos = rayPos + rayDir * t;
    vec3 uvw = pos - mapPos;
    vec3 wir = step(0.45, abs(uvw - 0.5));
    return (1.0 - wir.x * wir.y) * (1.0 - wir.x * wir.z) * (1.0 - wir.y * wir.z);
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2. - Resolution.xy) / Resolution.y;

    /* Ray Direction And Origin */
    vec3 rayDir = normalize((CameraView * vec4(uv * FieldOfView, 1.0, 0.0)).xyz);
    vec3 rayPos = CameraPosition / ParticleScale;

    ivec3 mapPos = ivec3(floor(rayPos + 0.0));
    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    bvec3 mask;

    // This must start as 0 or else things will be tinted by this color
    vec4 color = vec4(0.0);

    int iterations = 0;
    vec4 lastIterDidHitAndColor = vec4(-1.0);
    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        ++iterations;
        // Basically we hit something, so stop the loop, and because of the masking
        //   code below, a voxel will be rendered at the last position.
        vec4 tempColor = getParticle(mapPos);

        // We hit something!
        if (tempColor.r != -1.0) {

            if (lastIterDidHitAndColor.rgb != tempColor.rgb) {
                // Help with the alpha blending given by:
                //   https://github.com/Bowserinator/TPTBox

                float edge = 1.0;
                if (EnableOutlines == 1) {
                    edge = edgeCheck(rayPos, rayDir, mapPos);
                }

                float forwardAlphaInv = 1.0 - color.a; // data.color.a ???
                color.rgb += tempColor.rgb * (tempColor.a * forwardAlphaInv) * edge;
                color.a = 1.0 - forwardAlphaInv * (1.0 - tempColor.a);
            }

            if (tempColor.a == 1.0) break;
        }
        lastIterDidHitAndColor = tempColor;

        // -- Thanks kzy for the suggestion!
        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        // -- All components of mask are false except for the corresponding largest component
        // -- of sideDist, which is the axis along which the ray should be incremented.
        sideDist += vec3(mask) * deltaDist;
        mapPos += ivec3(vec3(mask)) * rayStep;
    }

    // If the max distance was reached, we need this.
    // Otherwise we get a voxel rendered at the max distance.
    if (iterations < MAX_RAY_STEPS) {
        if (EnableOutlines == 1) {
            float vvv = edgeCheck(rayPos, rayDir, mapPos);
            color.rgb *= vvv;
        }

        if (mask.x) {
            color.rgb *= vec3(0.5);
        }
        if (mask.y) {
            color.rgb *= vec3(1.0);
        }
        if (mask.z) {
            color.rgb *= vec3(0.75);
        }
    } else {
        // "Sky" color
        vec4 tempColor = vec4(0.1, 0.4, 0.5, 1.0);
        float forwardAlphaInv = 1.0 - color.a;
        color.rgb += tempColor.rgb * (tempColor.a * forwardAlphaInv);
        color.a = 1.0 - forwardAlphaInv * (1.0 - tempColor.a);
    }

    FragColor = color;
}
