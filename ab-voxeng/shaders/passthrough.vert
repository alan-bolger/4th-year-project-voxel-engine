#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 6) in mat4 matrixInstance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoords;
out vec3 fragPos;
out vec3 normal;

void main()
{
    texCoords = aTexCoords;
    fragPos = vec3(matrixInstance * vec4(aPos, 1.0)); // Uncomment this for instancing
    // fragPos = vec3(model * vec4(aPos, 1.0)); // Used for individual draw calls
    normal = transpose(inverse(mat3(matrixInstance))) * aNormal;
    gl_Position = projection * view * vec4(fragPos, 1.0);
} 