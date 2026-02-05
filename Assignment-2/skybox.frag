#version 330 core
out vec4 FragColor;

in vec3 localDir;
uniform sampler2D hdrMap;

const float PI = 3.14159265359;

vec2 dirToUV(vec3 d)
{
    d = normalize(d);
    return vec2(
        atan(d.z, d.x) / (2.0 * PI) + 0.5,
        asin(clamp(d.y, -0.9, 0.9)) / PI + 0.5
    );
}

void main()
{
    // slow rotation angle
    float t = 0.2 * 0.5; // tweak speed here

    mat3 rotY = mat3(
        cos(t), 0, -sin(t),
        0,      1,  0,
        sin(t), 0,  cos(t)
    );

    vec3 dir = rotY * localDir;
    vec3 hdr = texture(hdrMap, dirToUV(dir)).rgb;

    // exposure + tone mapping
    float exposure = 3.0;
    vec3 color = vec3(1.0) - exp(-hdr * exposure);

    FragColor = vec4(color, 1.0);
}

