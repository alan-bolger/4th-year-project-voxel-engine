#include "Game.h"
#include "SDL.h"
#include "Octree.h"

int main(int argc, char *argv[])
{
	// Initialise SDL here because it needs to be done BEFORE creating the window and renderer
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

	// Create Game object and start the game loop
	Game *m_game = new Game(1280, 720);
	m_game->start();

	// Delete Game object before exiting
	delete m_game;

	//Octree tree(1, 1, 1, 1024, 1024, 1024);
	
	//tree.insert(1, 2, 3);
	//tree.insert(1, 2, 3);
	//tree.insert(6, 5, 5);
	
	//std::cout << (tree.find(1, 2, 3) ? "Found\n" : "Not Found\n");
	
	//std::cout << (tree.find(3, 4, 4) ? "Found\n" : "Not Found\n");
	//tree.insert(3, 4, 4);
	
	//tree.insert(1024, 1024, 1024);
	
	//std::cout << (tree.find(1023, 1024, 1024) ? "Found\n" : "Not Found\n");
	
	// All is well
	return 1;
}