#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;

// GUI-controlled light parameters
uniform float lightAmbient;
uniform float lightDiffuse;

void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);

    float NdotL = max(dot(N, L), 0.0);

    // Quantised lighting levels
    float toonLevel;
    if (NdotL > 0.75)
        toonLevel = 1.0;
    else if (NdotL > 0.4)
        toonLevel = 0.6;
    else if (NdotL > 0.2)
        toonLevel = 0.3;
    else
        toonLevel = 0.15;

    // Ambient + toon diffuse
    vec3 ambient = lightAmbient * lightColor;
    vec3 diffuse = toonLevel * lightDiffuse * lightColor;

    vec3 color = ambient + diffuse;
    FragColor = vec4(color, 1.0);
}
