#version 460 core

// Try uncommenting 'noperspective' to see what happens.
/* noperspective  */in vec3 fragVertexColor;
/* noperspective  */in vec3 fragOrigin;
/* noperspective  */in vec3 fragDirection;

// This is an output variable that will be used by OpenGL
out vec4 FragColor;

uniform mat4 view;
uniform mat4 projection;

uniform uint EnableOutlines;


void main() {
    FragColor = vec4(fragVertexColor, 1.0);
}
