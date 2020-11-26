#include "Game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/// <summary>
/// Constructor for the Game class.
/// </summary>
/// <param name="t_width">- The width of the window.</param>
/// <param name="t_height">- The height of the window.</param>
Game::Game(int t_width, int t_height) 
	:
	m_window(SDL_CreateWindow("SDL2 Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, t_width, t_height, SDL_WINDOW_OPENGL)),
	m_renderer(SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED))
{
	// Create context for window
	m_glContext = SDL_GL_CreateContext(m_window);
	SDL_GL_MakeCurrent(m_window, m_glContext);

	// Initialise GLEW
	glewExperimental = GL_TRUE;
	auto init_res = glewInit();

	if (init_res != GLEW_OK)
	{
		std::cout << glewGetErrorString(glewInit()) << std::endl;
	}
	
	// Renderer settings
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255); // Set renderer colour to black	
	SDL_RenderSetLogicalSize(m_renderer, 1280, 720); // Set logical size for rendering
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // Set texture filtering to nearest pixel

	// Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;

	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		std::cout << "SDL_image could not initialize - SDL_image error: " << IMG_GetError();
	}

	// Initialize TTF loading
	if (TTF_Init() == -1)
	{
		std::cout << "SDL_ttf could not initialize - SDL_image error: " << TTF_GetError();
	}

	// Frame rate / Game loop
	m_frameRate = 60.0;
	m_frameMs = 1000.0f / m_frameRate;
	m_looping = true;

	// Raycasting test
	m_rayCastEng = new Raycaster(m_renderer);
}

/// <summary>
/// Destructor for the Game class.
/// </summary>
Game::~Game()
{
	SDL_DestroyWindow(m_window);
	m_window = NULL;

	SDL_DestroyRenderer(m_renderer);
	m_renderer = NULL;
	
	IMG_Quit();
	SDL_Quit();
}

/// <summary>
/// Start the game loop.
/// </summary>
void Game::start()
{
	Uint64 f_startMs;
	Uint64 f_endMs;
	Uint64 f_delayMs;

	while (m_looping)
	{		
		f_startMs = SDL_GetTicks();
		f_endMs = SDL_GetTicks();
		f_delayMs = m_frameMs - (f_endMs - f_startMs);
		SDL_Delay(f_delayMs);

		processEvents();
		update(f_delayMs);
		draw();
	}
}

/// <summary>
/// Process events.
/// </summary>
void Game::processEvents()
{
	SDL_Event f_event;
	Uint32 f_windowID = SDL_GetWindowID(m_window);

	while (SDL_PollEvent(&f_event))
	{
		switch (f_event.type)
		{
		case SDL_KEYDOWN:
			switch (f_event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				m_looping = false;
				break;
			}
			break;
		}
	}
}

/// <summary>
/// Update logic.
/// </summary>
void Game::update(double t_deltaTime)
{
	
}

/// <summary>
/// Draw.
/// </summary>
void Game::draw()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	// Clear screen
	SDL_RenderClear(m_renderer);

	m_rayCastEng->update();
	
	// Display everything
	SDL_RenderPresent(m_renderer);
}
