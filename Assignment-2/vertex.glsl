#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 WorldPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 camMatrix;   // view * projection

void main()
{
    vec4 world = model * vec4(aPos, 1.0);
    WorldPos = world.xyz;

    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = camMatrix * world;
}
