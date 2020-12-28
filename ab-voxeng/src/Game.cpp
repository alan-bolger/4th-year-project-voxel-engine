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

	// Put test stuff here so I can find it easily
	m_spheres.push_back(Sphere(glm::vec3(0.0, -10004, -20), 10000, glm::vec3(0.20, 0.20, 1.0), 0, 0.0));
	m_spheres.push_back(Sphere(glm::vec3(0.0, 0, -20), 4, glm::vec3(1.00, 0.32, 0.36), 1, 0.5));
	m_spheres.push_back(Sphere(glm::vec3(5.0, -1, -15), 2, glm::vec3(0.90, 0.76, 0.46), 1, 0.0));
	m_spheres.push_back(Sphere(glm::vec3(5.0, 0, -25), 3, glm::vec3(0.65, 0.77, 0.97), 1, 0.0));
	m_spheres.push_back(Sphere(glm::vec3(-5.5, 0, -15), 3, glm::vec3(0.90, 0.90, 0.90), 1, 0.0));
	m_spheres.push_back(Sphere(glm::vec3(0.0, 20, -30), 3, glm::vec3(0.00, 0.00, 0.00), 0, 0.0, glm::vec3(3))); // Light

	initialise();
}

/// <summary>
/// Destructor for the Game class.
/// </summary>
Game::~Game()
{
	SDL_DestroyWindow(m_window);
	m_window = NULL;

	delete m_controller;
	delete m_camera;
	delete m_mainShader;
	delete m_computeShader;
	
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
		update(f_delayMs);
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

	// Activate face culling
	// TODO: This isn't used because we're raytracing
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
	// TODO: Still not sure about v-sync
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
	m_frameRate = 30.0;
	m_frameMs = 1000.0f / m_frameRate;
	m_looping = true;

	// Controller
	m_controller = new ab::XboxOneController(0);

	// Camera
	m_camera = new ab::Camera(*m_controller);

	// Shaders
	m_mainShader = new ab::Shader("shaders/passthrough.vert", "shaders/passthrough.frag");
	m_renderQuadShader = new ab::Shader("shaders/renderquad.vert", "shaders/renderquad.frag");
	m_computeShader = new ab::Shader("shaders/raytracer.comp");

	// Terrain
	// m_terrain = new ab::Terrain();
	// m_terrain->generate(4, 4);

	// Ray tracer
	m_rayTracer = new ab::RayTracer();

	// Test model
	ab::OpenGL::import("models/generic-block.obj", m_cube);
	m_cube.matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	// Raytracing stuff
	m_FBOtextureID = ab::OpenGL::createFBO(1280, 720);
	initialiseRaytracing();

	// Bind quad VAO (this quad is used to render textures on)
	glGenVertexArrays(1, &m_quadVertexArrayObjectID);
	glBindVertexArray(m_quadVertexArrayObjectID);

	glGenBuffers(1, &m_quadVertexBufferObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBufferObjectID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_quadVertices), m_quadVertices, GL_STATIC_DRAW);

	m_cpuRayTracedTextureID = m_rayTracer->draw(m_spheres, *m_camera);
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

	// Update view and projection matrices
	ab::OpenGL::uniformMatrix4fv(*m_mainShader, "view", &m_camera->getView()[0][0]);
	ab::OpenGL::uniformMatrix4fv(*m_mainShader, "projection", &m_camera->getProjection()[0][0]);
}

/// <summary>
/// Draw.
/// </summary>
void Game::draw()
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shader
	// glUseProgram(m_mainShader->m_programID);

	// Draw test cube
	// ab::OpenGL::uniformMatrix4fv(*m_mainShader, "model", &m_cube.matrix[0][0]);
	// ab::OpenGL::draw(m_cube, m_mainShader, "diffuseTexture");

	// raytrace();	

	renderTextureToQuad(m_cpuRayTracedTextureID);
	
	// Display everything
	SDL_GL_SwapWindow(m_window);
}

/// <summary>
/// Initialises the compute shader.
/// </summary>
void Game::initialiseRaytracing()
{
	glUseProgram(m_computeShader->m_programID);

	GLint workGroupSize[3];
	glGetProgramiv(m_computeShader->m_programID, GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);
	m_workGroupSizeX = workGroupSize[0];
	m_workGroupSizeY = workGroupSize[1];

	glUseProgram(0);
}

/// <summary>
/// Perform raytracing.
/// </summary>
void Game::raytrace()
{
	glUseProgram(m_computeShader->m_programID);

	// Set viewing frustum corner rays in shader
	ab::OpenGL::uniform3f(*m_computeShader, "eye", m_camera->getEye().x, m_camera->getEye().y, m_camera->getEye().z);	

	m_camera->getEyeRay(-1, -1, m_eyeRay);
	ab::OpenGL::uniform3f(*m_computeShader, "ray00", m_eyeRay.x, m_eyeRay.y, m_eyeRay.z);

	m_camera->getEyeRay(-1, 1, m_eyeRay);
	ab::OpenGL::uniform3f(*m_computeShader, "ray01", m_eyeRay.x, m_eyeRay.y, m_eyeRay.z);

	m_camera->getEyeRay(1, -1, m_eyeRay);
	ab::OpenGL::uniform3f(*m_computeShader, "ray10", m_eyeRay.x, m_eyeRay.y, m_eyeRay.z);

	m_camera->getEyeRay(1, 1, m_eyeRay);
	ab::OpenGL::uniform3f(*m_computeShader, "ray11", m_eyeRay.x, m_eyeRay.y, m_eyeRay.z);

	// Bind level 0 of framebuffer texture as writable image in the shader
	glBindImageTexture(0, m_FBOtextureID, 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Compute appropriate invocation dimension
	int f_worksizeX = ab::OpenGL::nextPowerOfTwo(1280);
	int f_worksizeY = ab::OpenGL::nextPowerOfTwo(720);

	// Invoke the compute shader
	glDispatchCompute(f_worksizeX / m_workGroupSizeX, f_worksizeY / m_workGroupSizeY, 1);

	// Reset image binding
	glBindImageTexture(0, 0, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(0);

	// Render image on a quad
	renderTextureToQuad(m_FBOtextureID);
}

/// <summary>
/// Draw a quad and render a texture to it.
/// </summary>
void Game::renderTextureToQuad(GLuint &t_textureID)
{
	glUseProgram(m_renderQuadShader->m_programID);

	// Bind our texture in texture unit 1 and set shader to use texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, t_textureID);
	ab::OpenGL::uniform1i(*m_renderQuadShader, "uniformTexture", 1);

	// Vertex buffer object
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBufferObjectID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Disable vertex attribute array
	glDisableVertexAttribArray(0);
}
