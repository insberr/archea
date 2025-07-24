#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec2 u_position;    // Center position
uniform vec2 u_size;        // Width and height
uniform vec2 u_resolution;  // Screen resolution for normalization

void main()
{
    // Scale vertices by size and translate to position
    vec2 scaledPos = aPos * u_size;
    vec2 worldPos = scaledPos + u_position;

    // Convert to normalized device coordinates (-1 to 1)
    vec2 normalizedPos = (worldPos / u_resolution) * 2.0 - 1.0;

    gl_Position = vec4(normalizedPos, 0.0, 1.0);
}
