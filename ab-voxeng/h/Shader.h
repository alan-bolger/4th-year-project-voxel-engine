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
		~Shader();

	private:
		std::string readFile(const std::string &t_filePath);
		GLuint compileShader(GLuint t_type, const std::string &t_source);
		void createShader(const std::string &t_vertShader, const std::string &t_fragShader);
	};
}

#endif // !SHADER_H