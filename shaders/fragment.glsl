#version 330 core

out vec4 FragColor;
in vec3 color;

uniform vec2 Resolution;
uniform float Time;
uniform vec2 Mouse;

const int MAX_STEPS = 80;
const float MAX_DISTANCE = 100.0;
const float SURFACE_DISTANCE = 0.001;
const float FOV = 0.7;
const float MOUSE_SENSITIVITY = 4.0;

// 2D Rotation Function
mat2 rot2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

float cubeSDF(vec3 point, vec3 size) {
    vec3 q = abs(point) - size;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float scene(vec3 p) {

    vec3 q = fract(p) - .5;
    float cube = cubeSDF(q, vec3(0.2));

    float ground = p.y + .75;
    if (ground < -0.001) {
        ground = abs(ground);
    }

    return min(ground, cube);
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2. - Resolution.xy) / Resolution.y;
    vec2 m = (Mouse.xy * 2.0 - Resolution.xy) / Resolution.y * MOUSE_SENSITIVITY;

    vec3 rayOrigin = vec3(0.0, 0.0, -2.0);
    vec3 rayDirection = normalize(vec3(uv * FOV, 1.0));

    // Verticle movement
    rayOrigin.yz *= rot2D(-m.y);
    rayDirection.yz *= rot2D(-m.y);

    // Horizontal movement
    rayOrigin.xz *= rot2D(-m.x);
    rayDirection.xz *= rot2D(-m.x);

    vec3 col = vec3(0.0);

    float totalDistance = 0.0;
    for (int i = 0; i < MAX_STEPS; ++i) {
        vec3 p = rayOrigin + rayDirection * totalDistance;

        float d = scene(p);

        totalDistance += d;

        if (d < SURFACE_DISTANCE || totalDistance > MAX_DISTANCE) break;
    }

    col = vec3(totalDistance * 0.2);

    FragColor = vec4(col, 1.0);
}
