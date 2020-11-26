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

#include "Debug.h"
#include "RaycasterTest.h"

class Game
{
public:
	Game(int t_width, int t_height);
	~Game();
	void start();

private:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	SDL_GLContext m_glContext;
	double m_frameRate;
	double m_frameMs;
	bool m_looping;
	Raycaster *m_rayCastEng;

	void processEvents();
	void update(double t_deltaTime);
	void draw();
};

#endif // !GAME_H
