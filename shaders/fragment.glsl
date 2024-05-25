#version 460 core

/*
Credits to https://github.com/kallisto56/Raymarching-inside-cube for the code
*/



// Try uncommenting 'noperspective' to see what happens.
/* noperspective  */in vec3 fragVertexColor;
/* noperspective  */in vec3 fragOrigin;
/* noperspective  */in vec3 fragDirection;

// This is an output variable that will be used by OpenGL
out vec4 fragOutputColor;

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


void main ()
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
            fragOutputColor = vec4(point.xyz, 1);
            return;
        }

        // Advance the point along the ray direction using
        // value we've got from 'evaluateScene'.
        point += direction * distance;
    }

    // If we reached this point, it means we did not hit anything within COUNT_STEPS budget.
    // If you do not want to see quads, use "discard" operation.
    fragOutputColor = vec4(fragVertexColor.xyz, 1) * vec4(0.5, 0.5, 0.5, 1);
}
