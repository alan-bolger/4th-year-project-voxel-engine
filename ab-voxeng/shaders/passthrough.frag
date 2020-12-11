#version 330 core

uniform sampler2D diffuseTexture;

in vec2 texCoords;

out vec3 fragColour;

void main()
{
	fragColour = texture(diffuseTexture, texCoords).rgb;
}