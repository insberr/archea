#version 330 core

out vec4 FragColor;
in vec3 color;

uniform vec2 Resolution;
uniform float Time;

void main() {
    FragColor = vec4(color, 1.0);
}
