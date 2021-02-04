// ************************************************************************************
// * Shader.h and Shader.cpp contains snippets of code taken from the following site: *
// * http://www.opengl-tutorial.org													  *
// ************************************************************************************

#ifndef SHADER_H
#define SHADER_H

#include "glew/glew.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

namespace ab
{
	class Shader
	{
	public:
		GLuint m_programID;

		Shader(const std::string &t_vertShader, const std::string &t_fragShader);
		Shader(const std::string &t_computeShader);
		~Shader();

	private:
		std::string readFile(const std::string &t_filePath);
		GLuint compileShader(GLuint t_type, const std::string &t_source);
		void createShader(const std::string &t_vertShader, const std::string &t_fragShader);
		void createShader(const std::string &t_computeShader);
	};
}

#endif // !SHADER_H