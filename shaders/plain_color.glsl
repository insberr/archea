#version 460 core

in vec3 fragVertexColor;

uniform vec3 Color;

out vec4 FragColor;

void main() {
    // gl_FragDepth = 0.0;
    FragColor = vec4(fragVertexColor, 1.0);
}
