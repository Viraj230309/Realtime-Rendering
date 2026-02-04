#version 330 core
out vec4 FragColor;

in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D hdrMap;
uniform vec3 cameraPos;

const float PI = 3.14159265359;

// Direction → equirectangular UV
vec2 dirToUV(vec3 d)
{
    d = normalize(d);
    return vec2(
        atan(d.z, d.x) / (2.0 * PI) + 0.5,
        asin(d.y) / PI + 0.5
    );
}

float fresnelSchlick(float cosTheta)
{
    return pow(1.0 - cosTheta, 5.0);
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(cameraPos - WorldPos);

    // Reflection
    vec3 R = reflect(-V, N);
    vec3 reflection = texture(hdrMap, dirToUV(R)).rgb;

    // Chromatic dispersion refraction
    float etaR = 1.02;
    float etaG = 1.04;
    float etaB = 1.06;

    vec3 refrR = refract(-V, N, 1.0 / etaR);
    vec3 refrG = refract(-V, N, 1.0 / etaG);
    vec3 refrB = refract(-V, N, 1.0 / etaB);

    vec3 refraction;
    refraction.r = texture(hdrMap, dirToUV(refrR)).r;
    refraction.g = texture(hdrMap, dirToUV(refrG)).g;
    refraction.b = texture(hdrMap, dirToUV(refrB)).b;

    float F = fresnelSchlick(clamp(dot(V, N), 0.0, 1.0));
    vec3 glassColor = mix(refraction, reflection, F);

    // optional: slight absorption tint (VERY subtle)
    glassColor *= vec3(0.95, 0.98, 1.0);

    FragColor = vec4(glassColor, 1.0);

}
