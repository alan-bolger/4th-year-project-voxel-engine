#ifndef GAME_H
#define GAME_H

#include "glew/glew.h"
#include "glew/wglew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Debug.h"

#include "imgui-SFML.h"
#include "imgui.h"

#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class Game
{
public:
	Game(sf::ContextSettings t_settings);
	~Game();
	void run();
	
private:
	const unsigned int SCREEN_WIDTH = 1280;
	const unsigned int SCREEN_HEIGHT = 720;
	sf::RenderWindow m_window;
	sf::Time m_deltaTime;
	bool m_exitGame{ false };

	void initialise();
	void processEvents();
	void update(sf::Time t_deltaTime);
	void render();
};

#endif // !GAME_H

