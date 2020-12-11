#include "Game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/// <summary>
/// Constructor for the Game class.
/// </summary>
/// <param name="t_width">The width of the window.</param>
/// <param name="t_height">The height of the window.</param>
Game::Game(int t_width, int t_height) 
{
	m_window = SDL_CreateWindow("Voxel Engine [Alan Bolger]", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, t_width, t_height, SDL_WINDOW_OPENGL);
	initialise();
}

/// <summary>
/// Destructor for the Game class.
/// </summary>
Game::~Game()
{
	SDL_DestroyWindow(m_window);
	m_window = NULL;

	delete m_mainShader;
	
	IMG_Quit();
	SDL_Quit();
}

/// <summary>
/// Start the game loop.
/// </summary>
void Game::start()
{
	double f_startMs;
	double f_endMs;
	double f_delayMs;

	while (m_looping)
	{		
		f_startMs = SDL_GetTicks();
		f_endMs = SDL_GetTicks();
		f_delayMs = m_frameMs - (f_endMs - f_startMs);
		SDL_Delay((Uint32)f_delayMs);

		processEvents();
		update(f_delayMs / 100);
		draw();
	}
}

/// <summary>
/// Initialise everything.
/// </summary>
void Game::initialise()
{
	// Create context for window
	m_glContext = SDL_GL_CreateContext(m_window);
	SDL_GL_MakeCurrent(m_window, m_glContext);

	// Initialise GLEW
	glewExperimental = GL_TRUE;
	auto init_res = glewInit();

	// Activate super secret special OpenGL functions (not)
	// Culling faces will increase performance by around 50%
	// This should be disabled when drawing models that show front and back faces simultaneously
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Console message
	DEBUG_MSG("Voxel Engine");
	DEBUG_MSG("By Alan Bolger");
	DEBUG_MSG("----------------------------");
	DEBUG_MSG(glGetString(GL_VENDOR));
	DEBUG_MSG(glGetString(GL_RENDERER));
	DEBUG_MSG(glGetString(GL_VERSION));
	DEBUG_MSG("----------------------------");

	if (init_res != GLEW_OK)
	{
		std::cout << glewGetErrorString(glewInit()) << std::endl;
	}

	// Use v-sync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		std::cout << "Warning: Unable to set VSync! Error: %s\n", SDL_GetError();
	}

	// Renderer settings
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

	// View and projection matrices
	m_view = glm::mat4(1.0f);
	m_projection = glm::perspective(45.0f, 4.0f / 3.0f, 1.0f, 1000.0f);

	// Controller
	m_controller = new ab::XboxOneController(0);

	// Camera
	m_camera = new ab::Camera(*m_controller);

	// Shader
	m_mainShader = new ab::Shader("shaders/passthrough.vert", "shaders/passthrough.frag");

	// Test model
	ab::OpenGL::importModel("models/generic-block.obj", m_cube, "models/grass-block.png");
	m_cube.matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	// Uniforms for shader
	m_shaderModelMatrixID = glGetUniformLocation(m_mainShader->m_programID, "model");
	m_shaderViewMatrixID = glGetUniformLocation(m_mainShader->m_programID, "view");
	m_shaderProjectionMatrixID = glGetUniformLocation(m_mainShader->m_programID, "projection");
	m_diffuseTextureID = glGetUniformLocation(m_mainShader->m_programID, "diffuseTexture");
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

		m_controller->processEvents(f_event);
	}
}

/// <summary>
/// Update logic.
/// </summary>
/// <param name="t_deltaTime">The current delta time.</param>
void Game::update(double t_deltaTime)
{
	// Update camera and controls
	m_camera->update(t_deltaTime);

	// Update view matrix from camera
	glUniformMatrix4fv(m_shaderViewMatrixID, 1, GL_FALSE, &m_camera->getView()[0][0]);

	// Update projection matrix
	glUniformMatrix4fv(m_shaderProjectionMatrixID, 1, GL_FALSE, &m_projection[0][0]);
}

/// <summary>
/// Draw.
/// </summary>
void Game::draw()
{
	// Clear screen
	// glViewport(0, 0, 1280, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shader
	glUseProgram(m_mainShader->m_programID);

	// Draw test cube
	glUniformMatrix4fv(m_shaderModelMatrixID, 1, GL_FALSE, &m_cube.matrix[0][0]);

	// Draw
	ab::OpenGL::drawModel(m_cube);
	
	// Display everything
	SDL_GL_SwapWindow(m_window);
}