#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <windows.h>
#include <psapi.h>

#include "glew/glew.h"
#include "glew/wglew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Shader.h"
#include "OpenGL.h"
#include "Camera.h"
#include "XboxOneController.h"
#include "Terrain.h"

#include "Debug.h"

class Game
{
public:
	Game(int t_width, int t_height);
	~Game();
	void start();

private:
	SDL_Window *m_window;
	SDL_GLContext m_glContext;
	double m_frameRate;
	double m_frameMs;
	bool m_looping;
	ab::XboxOneController *m_controller;
	ab::Camera *m_camera;
	ab::Shader *m_mainShader;
	ab::Shader *m_renderQuadShader;
	ab::Shader *m_computeShader;
	// ab::Terrain *m_terrain;
	ab::Model m_cube;

	// Quad for render to texture
	GLuint m_quadVertexArrayObjectID;
	GLuint m_quadVertexBufferObjectID;

	// These vertices are used to draw a quad
	// The quad is used for rendering to a texture
	GLfloat m_quadVertices[18] =
	{
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	// Compute shader stuff
	GLint m_workGroupSizeX;
	GLint m_workGroupSizeY;
	GLuint m_frameBufferID;
	GLuint m_FBOtextureID;
	glm::vec3 m_eyeRay;

	void initialise();
	void processEvents();
	void update(double t_deltaTime);
	void draw();
	void initialiseRaytracing();
	void raytrace();
	void renderTextureToQuad();
};

#endif // !GAME_H
