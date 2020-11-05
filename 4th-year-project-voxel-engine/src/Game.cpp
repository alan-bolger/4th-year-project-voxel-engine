#include "Game.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/// <summary>
/// Constructor for the Game class.
/// </summary>
Game::Game(sf::ContextSettings t_settings) : m_window{ sf::VideoMode{ SCREEN_WIDTH, SCREEN_HEIGHT, 32 }, "4th Year Project", sf::Style::Default, t_settings }
{
	GLuint m_error = glewInit(); // Initialise GLEW
	initialise();
}

/// <summary>
/// Destructor for the Game class.
/// </summary>
Game::~Game()
{
	ImGui::SFML::Shutdown();
}

/// <summary>
/// Run game loop.
/// </summary>
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame = sf::seconds(1.f / 60.f);

	m_deltaTime = timePerFrame;

	while (m_window.isOpen() && !m_exitGame)
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();

		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			ImGui::SFML::Update(m_window, timePerFrame);
			update(timePerFrame);
			render();
		}
	}
}

/// <summary>
/// Initialise everything.
/// </summary>
void Game::initialise()
{
	ImGui::SFML::Init(m_window); // Initialise ImGUI window 

	DEBUG_MSG(glGetString(GL_VENDOR));
	DEBUG_MSG(glGetString(GL_RENDERER));
	DEBUG_MSG(glGetString(GL_VERSION));
}

/// <summary>
/// Process events
/// </summary>
void Game::processEvents()
{
	sf::Event event;

	while (m_window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}

		if (sf::Event::KeyPressed == event.type)
		{
			if (sf::Keyboard::Escape == event.key.code)
			{
				m_exitGame = true;
			}
		}
	}
}

/// <summary>
/// Update.
/// </summary>
void Game::update(sf::Time t_deltaTime)
{
	ImGui::Begin("4th Year Project");

	ImGui::Text("By Alan Bolger");

	ImGui::End();
}

/// <summary>
/// Render.
/// </summary>
void Game::render()
{
	// Reset viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render ImGui window
	m_window.pushGLStates();
	ImGui::SFML::Render(m_window);
	m_window.popGLStates();

	// Display everything
	m_window.display();
}

