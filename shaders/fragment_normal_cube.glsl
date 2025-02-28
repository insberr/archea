#version 460 core

in vec3 fragVertexColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(fragVertexColor, 1.0);
}
