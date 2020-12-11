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

#include "Debug.h"

class Game
{
public:
	Game(int t_width, int t_height);
	~Game();
	void start();

private:
	SDL_Window *m_window;
	//SDL_Renderer *m_renderer;
	SDL_GLContext m_glContext;
	double m_frameRate;
	double m_frameMs;
	bool m_looping;
	ab::XboxOneController *m_controller;
	ab::Camera *m_camera;
	ab::Shader *m_mainShader;
	ab::Model m_cube;
	glm::mat4 m_view;
	glm::mat4 m_projection;

	// Shader uniform IDs
	GLuint m_shaderModelMatrixID;
	GLuint m_shaderViewMatrixID;
	GLuint m_shaderProjectionMatrixID;
	GLuint m_diffuseTextureID;

	void initialise();
	void processEvents();
	void update(double t_deltaTime);
	void draw();
};

#endif // !GAME_H