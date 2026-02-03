#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 camPos;

// Material controls (GUI)
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

// Light controls (GUI)
uniform float lightAmbient;
uniform float lightDiffuse;
uniform float lightSpecular;

void main()
{
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);
    vec3 V = normalize(camPos - FragPos);
    vec3 H = normalize(L + V);

    // Ambient 
    vec3 ambient = lightAmbient * ambientStrength * lightColor;

    // Diffuse 
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = lightDiffuse * diff * lightColor;

    // Specular (Blinn-Phong) 
    float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specular = lightSpecular * specularStrength * spec * lightColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
