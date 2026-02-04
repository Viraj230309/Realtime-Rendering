#version 330 core
out vec4 FragColor;

in vec3 localPos;

uniform sampler2D hdrMap;

const float PI = 3.14159265359;

vec2 dirToUV(vec3 d)
{
    d = normalize(d);
    return vec2(
        atan(d.z, d.x) / (2.0 * PI) + 0.5,
        asin(d.y) / PI + 0.5
    );
}

void main()
{
    vec3 envColor = texture(hdrMap, dirToUV(localPos)).rgb;

    // simple tone mapping
    envColor = envColor / (envColor + vec3(1.0));

    FragColor = vec4(envColor, 1.0);
}
