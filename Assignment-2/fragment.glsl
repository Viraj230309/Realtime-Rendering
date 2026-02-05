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

    // ---------- Reflection ----------
    vec3 R = reflect(-V, N);
    vec3 reflection = texture(hdrMap, dirToUV(R)).rgb;

    // ---------- Chromatic dispersion ----------
    float etaR = 1.01;
    float etaG = 1.015;
    float etaB = 1.02;

    vec3 refrR = refract(-V, N, 1.0 / etaR);
    vec3 refrG = refract(-V, N, 1.0 / etaG);
    vec3 refrB = refract(-V, N, 1.0 / etaB);

    // Handle total internal reflection safely
    if (length(refrR) < 0.001) refrR = R;
    if (length(refrG) < 0.001) refrG = R;
    if (length(refrB) < 0.001) refrB = R;

    vec3 refraction;
    refraction.r = texture(hdrMap, dirToUV(refrR)).r;
    refraction.g = texture(hdrMap, dirToUV(refrG)).g;
    refraction.b = texture(hdrMap, dirToUV(refrB)).b;

    // ---------- Fresnel ----------
    float cosTheta = clamp(dot(V, N), 0.0, 1.0);
    float F0 = 0.04; // glass base reflectivity
    float F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    vec3 glassColor = mix(refraction * 0.8, reflection, F);



    // Subtle absorption tint
    glassColor *= vec3(0.95, 0.98, 1.0);

    // ---------- Tone mapping (CRITICAL) ----------
    glassColor = glassColor / (glassColor + vec3(1.0));
    glassColor = pow(glassColor, vec3(1.0 / 2.2)); // gamma correction


    FragColor = vec4(glassColor, 1.0);
}
