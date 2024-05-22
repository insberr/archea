#version 330 core

// Code copied and modified from https://www.shadertoy.com/view/4dX3zl
// And modified by me

//The raycasting code is somewhat based around a 2D raycasting toutorial found here:
//http://lodev.org/cgtutor/raycasting.html

out vec4 FragColor;
in vec3 fragCoord;

uniform vec2 Resolution;
uniform float Time;
uniform vec2 Mouse;
uniform usampler3D particles;

// 2D Rotation Function
mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

// Function to calculate distance from a point to a sphere
float sphereSDF(vec3 point, vec3 center, float radius) {
    return length(point - center) - radius;
}

float cubeSDF(vec3 point, vec3 size) {
    vec3 q = abs(point) - size;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

const int MAX_STEPS = 100;
const int MAX_RAY_STEPS = 64; // new
const float MAX_DISTANCE = 100.0;
// The distance to an object that is considered a hit
const float SURFACE_DIST = 0.001;
// FOV constant
const float FOV = 1.0;
const float MOUSE_SENSITIVITY = 4.0;

const vec3 colors[2] = vec3[2](
    vec3(0.5, 0.3, 0.2),
    vec3(0.2, 0.3, 0.7)
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

// test if a voxel exists here
vec3 getVoxel(ivec3 c) {
    vec3 none = vec3(-1.0);
    if (c.x > 3) return none;
    if (c.y > 3) return none;
    if (c.z > 3) return none;
    if (c.x < 0) return none;
    if (c.y < 0) return none;
    if (c.z < 0) return none;
    int val = data[c.z * (3 * 3) + c.y * (3) + c.x];

    // vec3 p = vec3(c) + vec3(0.5);
    // float d = sphereSDF(p, vec3(0), 5.0);
    // // uint value = texture(particles, c).r * 5u;

    if (val == 0) return none;

    return colors[val - 1];
}

float scene(vec3 p) {
    vec3 spherePos = vec3(sin(Time) * 3.0, 0, 0.0);
    float sphere = sphereSDF(p, spherePos, 1.0);

    vec3 q = fract(p) - .5;
    float cube = cubeSDF(q, vec3(.1));

    uint value = texture(particles, p).r;

    float ground = p.y + .75;
    if (ground < -0.001) {
        ground = abs(ground);
    }

    return min(ground, min(sphere, cube));
}

// The ray marching magic happens here
vec4 rayMarch(vec3 ro, vec3 rd) {
    float totalDistance = 0.0;
    vec3 color = vec3(0.0);

    for (int i = 0; i < MAX_STEPS; ++i) {
        vec3 p = ro + rd * totalDistance;

        float d = scene(p);

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

vec2 rotate2d(vec2 v, float a) {
    float sinA = sin(a);
    float cosA = cos(a);
    return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);
}

void main() {
    vec2 screenPos = (gl_FragCoord.xy / Resolution.xy) * 2.0 - 1.0;
    vec3 cameraDir = vec3(0.0, 0.0, 0.8);
    vec3 cameraPlaneU = vec3(1.0, 0.0, 0.0);
    vec3 cameraPlaneV = vec3(0.0, 1.0, 0.0) * Resolution.y / Resolution.x;
    vec3 rayDir = cameraDir + screenPos.x * cameraPlaneU + screenPos.y * cameraPlaneV;
    vec3 rayPos = vec3(0.0, 2.0 * sin(Time), -12.0);

     rayPos.xz = rotate2d(rayPos.xz, Time);
     rayDir.xz = rotate2d(rayDir.xz, Time);

    ivec3 mapPos = ivec3(floor(rayPos + 0.));

    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);

    ivec3 rayStep = ivec3(sign(rayDir));

    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayPos) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;

    bvec3 mask;

    vec3 color = vec3(0.0);

    int iterations = 0;
    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        ++iterations;
        // Basically we hit something, so stop the loop, and because of the masking
        //   code below, a voxel will be rendered at the last position.
        vec3 tempColor = getVoxel(mapPos);

        if (tempColor.r != -1.0) {
            color = tempColor;
            break;
        }


        //Thanks kzy for the suggestion!
        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        /*bvec3 b1 = lessThan(sideDist.xyz, sideDist.yzx);
        bvec3 b2 = lessThanEqual(sideDist.xyz, sideDist.zxy);
        mask.x = b1.x && b2.x;
        mask.y = b1.y && b2.y;
        mask.z = b1.z && b2.z;*/
        //Would've done mask = b1 && b2 but the compiler is making me do it component wise.

        //All components of mask are false except for the corresponding largest component
        //of sideDist, which is the axis along which the ray should be incremented.
        sideDist += vec3(mask) * deltaDist;
        mapPos += ivec3(vec3(mask)) * rayStep;
    }

    // vec3 color = vec3(1.0, 0.0, 0.0);
    if (mask.x) {
        color *= vec3(0.5);
    }
    if (mask.y) {
        color *= vec3(1.0);
    }
    if (mask.z) {
        color *= vec3(0.75);
    }

    // If the max distance was reached, we need this.
    // Otherwise we get a voxel rendered at the max distance.
    if (iterations == MAX_RAY_STEPS) color = vec3(0);

    FragColor = vec4(color, 1.0);
}
