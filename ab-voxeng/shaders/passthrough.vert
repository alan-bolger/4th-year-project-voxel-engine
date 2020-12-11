#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec2 texCoords;

void main()
{
    mat4 MVP = projection * view * model;
    texCoords = aTexCoords;
    gl_Position = MVP * vec4(aPos, 1.0);
} 