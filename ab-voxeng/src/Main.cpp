/// ********************************************************************************
/// * ModelLoader.cpp is based on code from the OpenGL Tutorial website.		   *
/// * http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/ *
/// ********************************************************************************
/// * Shader.cpp is based on code from the 


#include "Game.h"
#include "SDL.h"

int main(int argc, char *argv[])
{
	// Initialise SDL here because it needs to be done BEFORE creating the window and renderer
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

	// Set OpenGL version to use
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Create Game object and start the game loop
	Game *m_game = new Game(1280, 720);
	m_game->start();

	// Delete Game object before exiting
	delete m_game;
	
	// All is well
	return 1;
}