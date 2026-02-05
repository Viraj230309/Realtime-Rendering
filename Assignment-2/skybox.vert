#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 localDir;

uniform mat4 vp;

void main()
{
    localDir = aPos;
    gl_Position = vp * vec4(aPos, 1.0);
}
