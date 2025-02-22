#version 460 core

in vec3 fragVertexColor;
in vec3 fragOrigin;
in vec3 fragDirection;

uniform vec3 Color;

out vec4 FragColor;

void main() {
    // gl_FragDepth = 0.0;
    FragColor = vec4(Color, 1.0);
}
