#ifndef OPENGL_H
#define OPENGL_H

#include "glew/glew.h"
#include "glew/wglew.h"
#include "stb_image.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Shader.h"

namespace ab
{
	class OpenGL
	{
	public:
		static void import(const char *t_modelFilename, ab::Model &t_model, std::string t_diffuseTextureFilename = "");
		static void draw(ab::Model &t_model, Shader *t_shader, std::string t_uniformName = "");
		static GLuint createFBO(GLsizei t_width, GLsizei t_height);
		static int nextPowerOfTwo(int x);

		static void uniform1f(Shader &t_shader, std::string t_uniformName, float t_float);
		static void uniform2f(Shader &t_shader, std::string t_uniformName, float t_float_1, float t_float_2);
		static void uniform3f(Shader &t_shader, std::string t_uniformName, float t_float_1, float t_float_2, float t_float_3);
		static void uniform4f(Shader &t_shader, std::string t_uniformName, float t_float_1, float t_float_2, float t_float_3, float t_float_4);
		static void uniform1i(Shader &t_shader, std::string t_uniformName, GLint t_int);
		static void uniform2i(Shader &t_shader, std::string t_uniformName);
		static void uniform3i(Shader &t_shader, std::string t_uniformName);
		static void uniform4i(Shader &t_shader, std::string t_uniformName);		
		static void uniform1ui(Shader &t_shader, std::string t_uniformName);
		static void uniform2ui(Shader &t_shader, std::string t_uniformName);
		static void uniform3ui(Shader &t_shader, std::string t_uniformName);
		static void uniform4ui(Shader &t_shader, std::string t_uniformName);	
		static void uniform1fv(Shader &t_shader, std::string t_uniformName);
		static void uniform2fv(Shader &t_shader, std::string t_uniformName);
		static void uniform3fv(Shader &t_shader, std::string t_uniformName);
		static void uniform4fv(Shader &t_shader, std::string t_uniformName);		
		static void uniform1iv(Shader &t_shader, std::string t_uniformName);
		static void uniform2iv(Shader &t_shader, std::string t_uniformName);
		static void uniform3iv(Shader &t_shader, std::string t_uniformName);
		static void uniform4iv(Shader &t_shader, std::string t_uniformName);	
		static void uniform1uiv(Shader &t_shader, std::string t_uniformName);
		static void uniform2uiv(Shader &t_shader, std::string t_uniformName);
		static void uniform3uiv(Shader &t_shader, std::string t_uniformName);
		static void uniform4uiv(Shader &t_shader, std::string t_uniformName);	
		static void uniformMatrix2fv(Shader &t_shader, std::string t_uniformName);
		static void uniformMatrix3fv(Shader &t_shader, std::string t_uniformName);
		static void uniformMatrix4fv(Shader &t_shader, std::string t_uniformName, GLfloat *t_value);
		static void uniformMatrix2x3fv(Shader &t_shader, std::string t_uniformName);
		static void uniformMatrix3x2fv(Shader &t_shader, std::string t_uniformName);
		static void uniformMatrix2x4fv(Shader &t_shader, std::string t_uniformName);
		static void uniformMatrix4x2fv(Shader &t_shader, std::string t_uniformName);
		static void uniformMatrix3x4fv(Shader &t_shader, std::string t_uniformName);
		static void uniformMatrix4x3fv(Shader &t_shader, std::string t_uniformName);
	};
}

#endif // !OPENGL_H


