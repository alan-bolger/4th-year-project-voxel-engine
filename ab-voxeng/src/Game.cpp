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

	//// Put test stuff here so I can find it easily
	//m_spheres.push_back(Sphere(glm::vec3(0.0, -10004, -20), 10000, glm::vec3(0.20, 0.20, 1.0), 0, 0.0));
	//m_spheres.push_back(Sphere(glm::vec3(0.0, 0, -20), 4, glm::vec3(1.00, 0.32, 0.36), 1, 0.5));
	//m_spheres.push_back(Sphere(glm::vec3(5.0, -1, -15), 2, glm::vec3(0.90, 0.76, 0.46), 1, 0.0));
	//m_spheres.push_back(Sphere(glm::vec3(5.0, 0, -25), 3, glm::vec3(0.65, 0.77, 0.97), 1, 0.0));
	//m_spheres.push_back(Sphere(glm::vec3(-5.5, 0, -15), 3, glm::vec3(0.90, 0.90, 0.90), 1, 0.0));
	//m_spheres.push_back(Sphere(glm::vec3(0.0, 20, -30), 3, glm::vec3(0.00, 0.00, 0.00), 0, 0.0, glm::vec3(3))); // Light

	//glm::vec3 boundingBoxSize = glm::vec3(8, 8, 8);

	//m_cubes.push_back(Cube(glm::vec3(10.0, -10004, -20), glm::vec3(10000, 10000, 10000), glm::vec3(1, 1, 1), 0.0f, 0.f));
	//m_cubes.push_back(Cube(glm::vec3(-10, 2, 0), boundingBoxSize, glm::vec3(1.00, 0.32, 0.36), 0.0, 0.0));
	//m_cubes.push_back(Cube(glm::vec3(0, 4, 0), boundingBoxSize, glm::vec3(0.90, 0.76, 0.46), 0.5, 0.0));
	//m_cubes.push_back(Cube(glm::vec3(10, 6, 0), boundingBoxSize, glm::vec3(0.65, 0.77, 0.97), 0.5, 0.0));
	//m_cubes.push_back(Cube(glm::vec3(20, 8, 0), boundingBoxSize, glm::vec3(0.90, 0.90, 0.90), 0.5, 0.0));
	//m_cubes.push_back(Cube(glm::vec3(0.0, -10, 0), boundingBoxSize, glm::vec3(0.00, 0.00, 0.00), 0.0, 0.0, glm::vec3(3))); // Light

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
	delete m_renderQuadShader;
	delete m_computeShader;
	delete m_terrain;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	
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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	// window is the SDL_Window*
	// context is the SDL_GLContext
	ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
	ImGui_ImplOpenGL3_Init();

	// Activate face culling
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
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
	m_frameRate = 60.0;
	m_frameMs = 1000.0f / m_frameRate;
	m_looping = true;

	// Controller
	m_controller = new ab::XboxOneController(0);

	// Camera
	m_camera = new ab::Camera(*m_controller);
	m_cameraEye = m_camera->getEye();

	// Shaders
	m_mainShader = new ab::Shader("shaders/passthrough.vert", "shaders/passthrough.frag");
	m_renderQuadShader = new ab::Shader("shaders/renderquad.vert", "shaders/renderquad.frag");
	m_computeShader = new ab::Shader("shaders/raytracer.comp");

	// Terrain
	m_terrain = new ab::Terrain();
	m_terrain->generate(256, 256);

	// This is temp stuff for testing
	// glm::mat4 f_translationMatrix;

	for (int y = 0; y < 256; ++y)
	{
		for (int x = 0; x < 256; ++x)
		{
			m_cube.instancingPositions.push_back(glm::translate(glm::mat4(1.0f), m_terrain->m_boxes[y][x].center));
			m_voxelPositions.push_back(glm::vec4(m_terrain->m_boxes[y][x].center, 1.0));
		}
	}

	// Ray tracer
	// m_rayTracer = new ab::RayTracer();

	// Test model
	ab::OpenGL::import("models/generic-block.obj", m_cube, "models/grass-block.png");
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

	// Set directional light direction
	glUseProgram(m_mainShader->m_programID);
	ab::OpenGL::uniform3f(*m_mainShader, "dirLight.direction", m_directionalLightDirection.x, m_directionalLightDirection.y, m_directionalLightDirection.z);
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
		// Forward to ImGUI
		ImGui_ImplSDL2_ProcessEvent(&f_event);

		if (f_event.type == SDL_QUIT || (f_event.type == SDL_WINDOWEVENT && f_event.window.event == SDL_WINDOWEVENT_CLOSE
				&& f_event.window.windowID == SDL_GetWindowID(m_window)))
		{
			m_looping = false;
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
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window);
	ImGui::NewFrame();

	// Update camera and controls
	m_camera->update(t_deltaTime);

	// Update view and projection matrices
	ab::OpenGL::uniformMatrix4fv(*m_mainShader, "view", &m_camera->getView()[0][0]);
	ab::OpenGL::uniformMatrix4fv(*m_mainShader, "projection", &m_camera->getProjection()[0][0]);

	// ****************************
	// ** Dear ImGUI stuff below **
	// ****************************

	// Settings
	ImGui::Begin("SETTINGS");

	// Camera
	ImGui::Text("CAMERA");
	ImGui::Separator();
	ImGui::BulletText("Position");
	ImGui::InputFloat("X pos", &m_cameraEye.x);
	ImGui::InputFloat("Y pos", &m_cameraEye.y);
	ImGui::InputFloat("Z pos", &m_cameraEye.z);

	if (ImGui::Button("SET POSITION"))
	{
		m_camera->setEye(m_cameraEye);
	}

	ImGui::Separator();
	ImGui::BulletText("Current Position");
	ImGui::Text("X: %f", m_camera->getEye().x);
	ImGui::Text("Y: %f", m_camera->getEye().y);
	ImGui::Text("Z: %f", m_camera->getEye().z);
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Separator();

	// Directional light
	ImGui::Text("DIRECTIONAL LIGHT");
	ImGui::Separator();
	ImGui::BulletText("Direction");
	ImGui::InputFloat("X dir", &m_directionalLightDirection.x);
	ImGui::InputFloat("Y dir", &m_directionalLightDirection.y);
	ImGui::InputFloat("Z dir", &m_directionalLightDirection.z);

	if (ImGui::Button("SET DIRECTION"))
	{ 
		ab::OpenGL::uniform3f(*m_mainShader, "dirLight.direction", m_directionalLightDirection.x, m_directionalLightDirection.y, m_directionalLightDirection.z);
	}

	ImGui::Separator();
	ImGui::ColorPicker3("Ambient", m_directionalLightAmbient);
	ImGui::Separator();
	ImGui::ColorPicker3("Diffuse", m_directionalLightDiffuse);
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Separator();

	// Graphics settings
	const char *test_desc[] = { "Rasterization", "Raytracing" };
	ImGui::Checkbox("Wireframe Mode (only works with rasterization)", &m_wireframeMode);
	ImGui::Checkbox("Raytracing On", &m_raytracingOn);
	ImGui::End();

	// Update lighting parameters	
	ab::OpenGL::uniform3f(*m_mainShader, "dirLight.ambient", m_directionalLightAmbient[0], m_directionalLightAmbient[1], m_directionalLightAmbient[2]);
	ab::OpenGL::uniform3f(*m_mainShader, "dirLight.diffuse", m_directionalLightDiffuse[0], m_directionalLightDiffuse[1], m_directionalLightDiffuse[2]);
}

/// <summary>
/// Draw.
/// </summary>
void Game::draw()
{
	// Activate wireframe mode
	if (m_wireframeMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Turn on
	}

	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!m_raytracingOn)
	{
		// Activate shader
		glUseProgram(m_mainShader->m_programID);

		// Draw test cube
		ab::OpenGL::uniformMatrix4fv(*m_mainShader, "model", &m_cube.matrix[0][0]);
		ab::OpenGL::uniform3f(*m_mainShader, "viewPosition", m_camera->getEye().x, m_camera->getEye().y, m_camera->getEye().z);
		ab::OpenGL::draw(m_cube, m_mainShader, "diffuseTexture");
	}
	else
	{
		raytrace();
	}	
	
	// Render ImGUI stuff
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	
	
	// Display everything
	SDL_GL_SwapWindow(m_window);

	// Deactivate wireframe mode
	if (m_wireframeMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Turn off
	}
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

	// Copy voxel positions to GPU
	glGenBuffers(1, &m_voxelPositions_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_voxelPositions_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_voxelPositions.size() * sizeof(glm::vec4), &m_voxelPositions[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_voxelPositions_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

	// Voxel buffer object
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_voxelPositions_SSBO);
	// glBufferData(GL_SHADER_STORAGE_BUFFER, m_voxelPositions.size() * sizeof(glm::vec3), &m_voxelPositions[0], GL_READ_ONLY);

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
