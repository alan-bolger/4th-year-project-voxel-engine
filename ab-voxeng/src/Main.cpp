#include "Game.h"
#include "SDL.h"

int main(int argc, char *argv[])
{
	// Initialise SDL here because it needs to be done BEFORE creating the window and renderer
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

	// Create Game object and start the game loop
	Game *m_game = new Game(1280, 720);
	m_game->start();

	// Delete Game object before exiting
	delete m_game;
	
	// All is well
	return 1;
}