#version 410 core

layout (location = 0) in vec3 aPos;

out vec2 TexCoords;

void main(void) 
{
	gl_Position = vec4(aPos.xy, 0.0, 1.0);
	TexCoords = aPos.xy * 0.5 + vec2(0.5, 0.5);
}
