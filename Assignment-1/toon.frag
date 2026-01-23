#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float intensity = dot(norm, lightDir);

    float levels;
    if (intensity > 0.95)
        levels = 1.0;
    else if (intensity > 0.5)
        levels = 0.7;
    else if (intensity > 0.25)
        levels = 0.4;
    else
        levels = 0.1;

    vec3 toonColor = levels * lightColor;
    FragColor = vec4(toonColor
