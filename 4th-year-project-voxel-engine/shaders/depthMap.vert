#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 6) in mat4 matrixInstance;

uniform mat4 M;
uniform mat4 lightSpaceMatrix;
uniform bool useInstancing;

void main()
{
    if (useInstancing)
    {
        gl_Position = lightSpaceMatrix * matrixInstance * vec4(aPos, 1.0);
    }
    else
    {
        gl_Position = lightSpaceMatrix * M * vec4(aPos, 1.0);
    }
} 