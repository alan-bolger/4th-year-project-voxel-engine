#ifndef MODEL_H
#define MODEL_H

#include "glm/glm.hpp"
#include "SDL_opengl.h"

#include <vector>

struct Model
{
	GLuint vertexArrayObjectID;
	GLuint vertexBufferID;
	GLuint uvBufferID;
	GLuint normalBufferID;
	GLuint diffuseTextureID;
	GLuint elementBufferID;
	GLuint instanceBufferID;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned short> indices;
	std::vector<glm::mat4> instancingPositions;
	glm::mat4 matrix;
};

#endif // !MODEL_H
