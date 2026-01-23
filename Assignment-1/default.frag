#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D tex0;        
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 emissionColor;    // e.g., vec3(1.0, 0.35, 0.05)
uniform vec3 atmosphereColor;  // e.g., vec3(1.0, 0.5, 0.1)

float luminance(vec3 c) { return dot(c, vec3(0.2126, 0.7152, 0.0722)); }

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

uniform DirectionalLight dirLight;


void main()
{
    // --- Sample base color (Planet.png) ---
    vec3 base = texture(tex0, TexCoord).rgb;

    // Work in linear-ish space to avoid overbright issues
    base = pow(base, vec3(2.2));

    // --- Lighting setup ---
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);

    float NdotL = max(dot(N, L), 0.0);
    float spec   = pow(max(dot(V, R), 0.0), 24.0);

    // --- Separate lava from rock using the orange-ness heuristic ---
    float orange = clamp(base.r - base.b, 0.0, 1.0);

    // Clean mask: threshold + soft edge
    // tweak t and w if lines are too thin/thick
    float t = 0.20;   // threshold
    float w = 0.10;   // softness
    float lavaMask = smoothstep(t, t + w, orange);

    // Keep rock darker, don’t let lava tint the whole surface
    vec3 rock = mix(base, base * 0.55, lavaMask * 0.25);

    // --- Classic Phong terms (muted a bit) ---
    vec3 ambient  = 0.17 * rock;
    vec3 diffuse  = NdotL * rock * lightColor * 0.9;
    vec3 specular = spec * lightColor * 0.12;

    // --- Emission only in lava cracks ---
    float viewFacing = clamp(0.5 + 0.5 * dot(V, N), 0.0, 1.0);
    vec3 emission = emissionColor * lavaMask * 1.8 * viewFacing;

    // --- Rim glow (soft atmosphere) near silhouette ---
    float rim = 1.0 - max(dot(V, N), 0.0);          // 0 center → 1 edge
    vec3 atmosphere = atmosphereColor * pow(rim, 3.0) * 0.15;

    // --- Combine & return to gamma space ---
    vec3 colorLin = ambient + diffuse + specular + emission + atmosphere;
    vec3 colorSRGB = pow(colorLin, vec3(1.0/2.2));

    FragColor = vec4(colorSRGB, 1.0);
}
