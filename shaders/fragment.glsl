#version 460 core

/*
Credits to https://github.com/kallisto56/Raymarching-inside-cube for the code
*/

// Try uncommenting 'noperspective' to see what happens.
/* noperspective  */in vec3 fragVertexColor;
/* noperspective  */in vec3 fragOrigin;
/* noperspective  */in vec3 fragDirection;

// This is an output variable that will be used by OpenGL
out vec4 FragColor;



layout (binding = 0, std430) readonly restrict buffer Particles {
    int particles[];
};
layout (binding = 1, std430) readonly restrict buffer Colors {
    vec4 colors[];
};

uniform mat4 view;
uniform uint EnableOutlines;
// The number of ray steps to make
uniform int MAX_RAY_STEPS;


// In this situation, epsilon is the smallest acceptable number,
// below which, we conclude that we hit the surface of the SDF volume.
// Change this value to 0.1 to see what happens.
#define EPSILON 0.001

// The more complex scene is, the more steps you will need.
// This is especially prominent when camera is at the center
// of this particular model.
#define COUNT_STEPS 100


float opSubtraction(float d1, float d2) // Inigo Quilez (https://iquilezles.org/articles/distfunctions)
{
    return max(-d1, d2);
}


float opSmoothUnion(float d1, float d2, float k) // Inigo Quilez (https://iquilezles.org/articles/distfunctions)
{
    float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return mix( d2, d1, h) - k * h * (1.0 - h);
}


float sdSphere (vec3 point, vec3 sphere, float radius) // Inigo Quilez (https://iquilezles.org/articles/distfunctions)
{
    return length(point - sphere.xyz) - radius;
}


float sdBox(vec3 point, vec3 bounds) // Inigo Quilez (https://iquilezles.org/articles/distfunctions)
{
    vec3 q = abs(point) - bounds;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}


float evaluateScene (vec3 point)
{
    // uncomment this line to see what happens,
    // when object goes outside of the volume.
    // return sdSphere(point, vec3(0, 0, 1), 0.5);
    float box = sdBox(point, vec3(0.38));
    float sphere = sdSphere(point, vec3(0, 0, 0), 0.5);
    float lhs = opSubtraction(sphere, box);
    // return lhs; // uncomment to see what this group of instructions is about

    float blob0 = sdSphere(point, vec3(0, 0.0, +0.25), 0.1);
    float blob1 = sdSphere(point, vec3(0, 0.0, -0.25), 0.1);
    float blob2 = sdSphere(point, vec3(+0.25, 0.0, 0.0), 0.1);
    float blob3 = sdSphere(point, vec3(-0.25, 0.0, 0.0), 0.1);
    float blob4 = sdSphere(point, vec3(0.0, -0.25, 0.0), 0.1);
    float blob5 = sdSphere(point, vec3(0.0, 0.25, 0.0), 0.1);

    float rhs = opSmoothUnion(blob0, blob1, 0.59);
    // return rhs; // uncomment to see what this group of instructions is about

    rhs = opSmoothUnion(rhs, blob2, 0.15);
    rhs = opSmoothUnion(rhs, blob3, 0.15);
    rhs = opSmoothUnion(rhs, blob4, 0.15);
    rhs = opSmoothUnion(rhs, blob5, 0.15);
    // return rhs; // uncomment to see what this group of instructions is about

    return min(lhs, rhs);
}


vec2 intersectAABB (vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax)
{
    // taken from: https://gist.github.com/DomNomNom/46bb1ce47f68d255fd5d
    // which was adapted from https://github.com/evanw/webgl-path-tracing/blob/master/webgl-path-tracing.js
    vec3 tMin = (boxMin - rayOrigin) / rayDir;
    vec3 tMax = (boxMax - rayOrigin) / rayDir;

    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);

    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);

    return vec2(tNear, tFar);
}


void main_old ()
{
    // Try commenting 'normalize' operation and then move
    // camera around object to see what happens.
    vec3 direction = normalize(fragDirection);
    vec3 point = fragOrigin;

    // Uncomment this line to move a ray inside the cube
    // point = point + direction * max(0, intersectAABB(point, direction, vec3(-0.5), vec3(+0.5)).x);

    // ...
    for (int n = 0; n < COUNT_STEPS; n++)
    {
        float distance = evaluateScene(point);

        // Are we close enough to surface?
        if (distance < EPSILON)
        {
            FragColor = vec4(point.xyz, 1);
            return;
        }

        // Advance the point along the ray direction using
        // value we've got from 'evaluateScene'.
        point += direction * distance;
    }

    // If we reached this point, it means we did not hit anything within COUNT_STEPS budget.
    // If you do not want to see quads, use "discard" operation.
    FragColor = vec4(fragVertexColor.xyz, 1) * vec4(0.5, 0.5, 0.5, 1);
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
    /* Ray Direction And Origin */
    // Try commenting 'normalize' operation and then move
    // camera around object to see what happens.
    vec3 rayDir = normalize(fragDirection);
    vec3 rayPos = fragOrigin;

    // Uncomment this line to move a ray inside the cube
    // rayPos = rayPos + rayDir * max(0, intersectAABB(rayPos, rayDir, vec3(-0.5), vec3(+0.5)).x);


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

                //                float edge = 1.0;
                //                if (EnableOutlines == 1) {
                //                    edge = edgeCheck(rayPos, rayDir, mapPos);
                //                }

                float forwardAlphaInv = 1.0 - color.a; // data.color.a ???
                color.rgb += tempColor.rgb * (tempColor.a * forwardAlphaInv); // * edge;
                color.a = 1.0 - forwardAlphaInv * (1.0 - tempColor.a);

                if (EnableOutlines == 1 && tempColor.a != 1.0) {
                    float edge = edgeCheck(rayPos, rayDir, mapPos);
                    if (edge != 1.0) {
                        color.rgb = vec3(edge);
                        color.a = 1.0;
                    }
                }
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
            if (vvv != 1.0) color.rgb = vec3(vvv);
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

    // Depth Buffer
    // -- https://stackoverflow.com/a/29397319/6079328
    const float DEPTH_FAR_AWAY = 1000.0;
    vec4 vClipCoord = view * vec4(mapPos, 1.0);
    float fNdcDepth = vClipCoord.z / vClipCoord.w;
    gl_FragDepth = color.a > 0.0 ? (fNdcDepth + 1.0) * 0.5 : DEPTH_FAR_AWAY;

    FragColor = color;
}
