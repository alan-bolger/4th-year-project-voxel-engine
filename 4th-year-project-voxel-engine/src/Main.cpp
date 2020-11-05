//#ifdef _DEBUG 
//#pragma comment(lib,"sfml-graphics-d.lib") 
//#pragma comment(lib,"sfml-audio-d.lib") 
//#pragma comment(lib,"sfml-system-d.lib") 
//#pragma comment(lib,"sfml-window-d.lib") 
//#pragma comment(lib,"sfml-network-d.lib") 
//#else 
//#pragma comment(lib,"sfml-graphics.lib") 
//#pragma comment(lib,"sfml-audio.lib") 
//#pragma comment(lib,"sfml-system.lib") 
//#pragma comment(lib,"sfml-window.lib") 
//#pragma comment(lib,"sfml-network.lib") 
//#endif

#include "Game.h"

int main()
{
	// Setup SFML context settings
	sf::ContextSettings f_settings;
	f_settings.depthBits = 24u; // For depth buffer (Z buffer)
	f_settings.antialiasingLevel = 4u;
	f_settings.majorVersion = 4;
	f_settings.minorVersion = 1;

	// Create a reference to the game and run
	Game *f_game = new Game(f_settings);
	f_game->run();

	delete f_game;

	return 1;
}