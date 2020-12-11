#ifndef OPENGL_H
#define OPENGL_H

#include "glew/glew.h"
#include "glew/wglew.h"
#include "Model.h"
#include "ModelLoader.h"

namespace ab
{
	class OpenGL
	{
	public:
		static void importModel(const char *t_modelFilename, Model &t_model, bool t_instancingOn);
		static void drawModel(Model &t_model);
		static void drawModelsInstanced(Model &t_model);

	private:

	};
}

#endif // !OPENGL_H


