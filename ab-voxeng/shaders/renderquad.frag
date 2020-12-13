#version 330 core

in vec2 TexCoords;

out vec3 colour;

uniform sampler2D uniformTexture;

void main()
{
	colour = texture(uniformTexture, TexCoords).xyz;
}