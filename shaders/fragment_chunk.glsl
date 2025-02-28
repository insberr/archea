#version 460 core

in vec4 fragVertexColor;
out vec4 FragColor;

void main() {
    FragColor = fragVertexColor;
}
