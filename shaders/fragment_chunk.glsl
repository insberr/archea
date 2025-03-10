#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 Color;

uniform float particleScale;

const vec3 lightColor = vec3(1.0);

void main()
{
    vec3 lightPos = vec3(100, 70, 50);

    // ambient
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
//    float specularStrength = 0.5;
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = specularStrength * spec * lightColor;

    vec4 result = vec4(ambient + diffuse, 1.0) * Color;
    FragColor = result;
}
