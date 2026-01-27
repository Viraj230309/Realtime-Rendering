#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;

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
        toonLevel = 0.7;
    else if (NdotL > 0.2)
        toonLevel = 0.4;
    else
        toonLevel = 0.2;

    // Ambient term (CRITICAL)
    vec3 ambient = vec3(0.15);

    vec3 color = ambient + toonLevel * lightColor;
    FragColor = vec4(color, 1.0);
}
