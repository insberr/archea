#version 330 core

// Code copied and modified from https://www.shadertoy.com/view/4dX3zl
// -- The raycasting code is somewhat based around a 2D raycasting toutorial found here:
// -- http://lodev.org/cgtutor/raycasting.html

out vec4 FragColor;
in vec3 fragCoord;

uniform vec2 Resolution;
uniform float Time;
uniform vec2 Mouse;

// todo Make these uniform arrays passed from the cpu
const vec4 colors[2] = vec4[2](
    vec4(0.5, 0.3, 0.2, 1.0),
    vec4(0.2, 0.3, 0.7, 0.5)
);
const int data[64] = int[64](
    0, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,

    2, 0, 2, 0,
    0, 0, 0, 0,
    1, 0, 1, 0,
    0, 0, 0, 0,

    0, 0, 0, 0,
    1, 0, 1, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,

    0, 0, 0, 0,
    1, 0, 1, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
);

// Constants
// The number of ray steps to make
const int MAX_RAY_STEPS = 100;

// Not sure if this is needed
const float MAX_DISTANCE = 100.0;

// The distance to an object that is considered a hit
const float SURFACE_DIST = 0.001;

// FOV constant
const float FOV = 1.0; // todo add this??
const float MOUSE_SENSITIVITY = 4.0;

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
// test if a voxel exists here
vec4 getParticle(ivec3 c) {
    // Ground
    // if (c.y == -1.0) return vec4(1.0);
    // X axis
    if (c.y == -1.0 && c.z == -1.0) return vec4(1.0, 0.0, 0.0, 1.0); // x axis
    if (c.x == -1.0 && c.z == -1.0) return vec4(0.0, 1.0, 0.0, 1.0); // y axis
    if (c.x == -1.0 && c.y == -1.0) return vec4(0.0, 0.0, 1.0, 1.0); // z axis

    // Array index range checks
    if (c.x > 3) return NoParticle;
    if (c.y > 3) return NoParticle;
    if (c.z > 3) return NoParticle;
    if (c.x < 0) return NoParticle;
    if (c.y < 0) return NoParticle;
    if (c.z < 0) return NoParticle;

    // Get the value in the array
    // z * (ysize * xsize) + y * (xsize) + x
    int val = data[c.z * (3 * 3) + c.y * (3) + c.x];

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

void main() {
    vec2 screenPos = (gl_FragCoord.xy / Resolution.xy) * 2.0 - 1.0;
    vec2 m = (Mouse.xy / Resolution.xy) * 2.0 - 1.0;
    m *= MOUSE_SENSITIVITY;
    vec3 cameraDir = vec3(0.0, 0.0, 0.8);
    vec3 cameraPlaneU = vec3(1.0, 0.0, 0.0);
    vec3 cameraPlaneV = vec3(0.0, 1.0, 0.0) * Resolution.y / Resolution.x;
    vec3 rayDir = cameraDir + screenPos.x * cameraPlaneU + screenPos.y * cameraPlaneV;
    vec3 rayPos = vec3(2.0, 2.0, -8.0);

    rayPos.yz = rotate2d(rayPos.yz, -m.y);
    rayDir.yz = rotate2d(rayDir.yz, -m.y);

    rayPos.xz = rotate2d(rayPos.xz, -m.x);
    rayDir.xz = rotate2d(rayDir.xz, -m.x);

    ivec3 mapPos = ivec3(floor(rayPos + 0.));

    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);

    ivec3 rayStep = ivec3(sign(rayDir));

    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;

    bvec3 mask;

    vec4 color = vec4(0.0);

    int iterations = 0;
    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        ++iterations;
        // Basically we hit something, so stop the loop, and because of the masking
        //   code below, a voxel will be rendered at the last position.
        vec4 tempColor = getParticle(mapPos);

        // We hit something
        if (tempColor.r != -1.0) {
            // Help with the alpha blending given by:
            //   https://github.com/Bowserinator/TPTBox
            float forwardAlphaInv = 1.0 - color.a; // data.color.a ???
            color.rgb += tempColor.rgb *  (tempColor.a * forwardAlphaInv);
            color.a = 1.0 - forwardAlphaInv * (1.0 - tempColor.a);

            if (tempColor.a == 1.0) break;
        }

        // Thanks kzy for the suggestion!
        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));

        //All components of mask are false except for the corresponding largest component
        //of sideDist, which is the axis along which the ray should be incremented.
        sideDist += vec3(mask) * deltaDist;
        mapPos += ivec3(vec3(mask)) * rayStep;
    }

    if (iterations < MAX_RAY_STEPS) {
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

    // If the max distance was reached, we need this.
    // Otherwise we get a voxel rendered at the max distance.
    // if (iterations == MAX_RAY_STEPS) color = vec4(0.0);

    FragColor = color;
}
