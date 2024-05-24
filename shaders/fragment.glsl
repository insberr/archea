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

// todo Make these uniform arrays passed from the cpu
//const vec4 colors[2] = vec4[2](
//    vec4(0.5, 0.3, 0.2, 1.0),
//    vec4(0.2, 0.3, 0.7, 0.7)
//);
//const int data[64] = int[64](
//    0, 0, 0, 0,
//    0, 1, 0, 0,
//    0, 0, 0, 0,
//    0, 0, 0, 0,
//
//    2, 0, 2, 0,
//    0, 0, 0, 0,
//    1, 0, 1, 0,
//    0, 0, 0, 0,
//
//    0, 0, 0, 0,
//    1, 0, 1, 0,
//    0, 0, 0, 0,
//    0, 0, 0, 0,
//
//    0, 0, 0, 0,
//    1, 0, 1, 0,
//    0, 0, 0, 0,
//    0, 0, 0, 0
//);

// Constants
// The number of ray steps to make
uniform int MAX_RAY_STEPS; // = 100;

// Not sure if this is needed
const float MAX_DISTANCE = 100.0;

// The distance to an object that is considered a hit
const float SURFACE_DIST = 0.001;
const float voxelScale = 0.2;

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
const vec4 AXIS_PARTICLE = vec4(1.0, 0.0, 0.0, 1.0);
const int arraySize = 50;
// test if a voxel exists here
vec4 getParticle(vec3 _c) {
    ivec3 c = ivec3(_c / (1.0 - voxelScale));
    // Ground
    if (c.y == -2.0) return vec4(vec3(0.5), 0.6); // 0.6
    // X axis
    if (c.y == -1.0 && c.z == -1.0) return vec4(1.0, 0.0, 0.0, 0.4); // x axis // 0.4
    if (c.x == -1.0 && c.z == -1.0) return vec4(0.0, 1.0, 0.0, 0.4); // y axis
    if (c.x == -1.0 && c.y == -1.0) return vec4(0.0, 0.0, 1.0, 0.4); // z axis

    // Array index range checks
    if (c.x > arraySize - 1) return NoParticle;
    if (c.y > arraySize - 1) return NoParticle;
    if (c.z > arraySize - 1) return NoParticle;
    if (c.x < 0) return NoParticle;
    if (c.y < 0) return NoParticle;
    if (c.z < 0) return NoParticle;

    // Get the value in the array
    // z * (ysize * xsize) + y * (xsize) + x
    int val = particles[c.z * (arraySize * arraySize) + c.y * (arraySize) + c.x];

    if (val == 0) return NoParticle;

    return colors[val - 1];
}

// The ray marching magic happens here
vec4 rayMarch(vec3 ro, vec3 rd) {
    float totalDistance = 0.0;
    vec3 color = vec3(0.0);

    for (int i = 0; i < MAX_RAY_STEPS; ++i) {
        vec3 p = ro + rd * totalDistance;

        float d = 0.0; // scene(p);

        totalDistance += d;

        // Color based on iteration count
        // color = vec3(i) / MAX_STEPS;

        if (d < SURFACE_DIST || totalDistance > MAX_DISTANCE) break;
    }

    // Color based on depth
    color = vec3(totalDistance * .2);

    return vec4(color, 1.0);

    //    if (totalDistance > MAX_DISTANCE) {
    //        return vec4(0.0);
    //    } else {
    //            return vec4(0.7);
    //    }

    // return vec4(0.0);
}

float edgeCheck(vec3 rayPos, vec3 rayDir, vec3 mapPos) {
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
    vec2 uv = (gl_FragCoord.xy * 2. - Resolution.xy) / Resolution.y; // new

    /* Ray Direction And Origin */
    vec3 rayDir = normalize((CameraView * vec4(uv * FieldOfView, 1.0, 0.0)).xyz);
    vec3 rayPos = CameraPosition;

    vec3 mapPos = floor(rayPos + 0.);
    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    vec3 rayStep = sign(rayDir * voxelScale);
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;
    bvec3 mask;

    vec4 color = vec4(0.0);

    int iterations = 0;
    int timesThroughVoxel = 0;
    vec4 lastIterDidHitAndColor = vec4(-1.0);
    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        ++iterations;
        // Basically we hit something, so stop the loop, and because of the masking
        //   code below, a voxel will be rendered at the last position.
        vec4 tempColor = getParticle(mapPos);

        // We hit something
        if (tempColor.r != -1.0) {

            if (lastIterDidHitAndColor.rgb != tempColor.rgb) {
                // Help with the alpha blending given by:
                //   https://github.com/Bowserinator/TPTBox
                float edge = edgeCheck(rayPos, rayDir, mapPos);

                float forwardAlphaInv = 1.0 - color.a; // data.color.a ???
                color.rgb += tempColor.rgb * (tempColor.a * forwardAlphaInv); // * edge;
                color.a = 1.0 - forwardAlphaInv * (1.0 - tempColor.a);
            }

            timesThroughVoxel++;

            if (tempColor.a == 1.0) break;
        }
        lastIterDidHitAndColor = tempColor;

        // Thanks kzy for the suggestion!
        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));

        //All components of mask are false except for the corresponding largest component
        //of sideDist, which is the axis along which the ray should be incremented.
        sideDist += vec3(mask) * deltaDist;
        // This looks really weird, use at own risk
        // if (fract(sideDist.x) < 0.02) color = vec4(1.0, 0.0, 0.0, 1.0);
        mapPos += vec3(mask) * rayStep;
    }

    float vvv = edgeCheck(rayPos, rayDir, mapPos);

    // If the max distance was reached, we need this.
    // Otherwise we get a voxel rendered at the max distance.
    if (iterations < MAX_RAY_STEPS) {
        // color.rgb *= vvv;
        // vec3 color = vec3(1.0, 0.0, 0.0);
        if (mask.x) {
            color.rgb *= vec3(0.5);
        }
        if (mask.y) {
            color.rgb *= vec3(1.0);
        }
        if (mask.z) {
            color.rgb *= vec3(0.75);
        }
    }

    FragColor = color;
}
