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
	SCREEN_WIDTH = t_width;
	SCREEN_HEIGHT = t_height;
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

	delete m_controller;
	delete m_camera;
	delete m_mainShader;
	delete m_renderQuadShader;
	delete m_computeShader;
	delete m_skyboxShader;

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
	// m_window is the SDL_Window*
	// m_context is the SDL_GLContext
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
	// TODO: Still not sure about v-sync - Can I toggle this during runtime?
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
	m_controller = new ab::XboxOneController(0); // Zero is the controller index value

	// Camera
	m_camera = new ab::Camera(*m_controller);
	m_cameraEye = m_camera->getEye();

	// Shaders
	m_mainShader = new ab::Shader("shaders/passthrough.vert", "shaders/passthrough.frag");
	m_renderQuadShader = new ab::Shader("shaders/renderquad.vert", "shaders/renderquad.frag");
	m_computeShader = new ab::Shader("shaders/raytracer.comp");
	m_skyboxShader = new ab::Shader("shaders/skybox.vert", "shaders/skybox.frag");

	// Terrain and map population
	m_terrain = new ab::Terrain();
	m_terrain->generate(MAP_WIDTH, MAP_DEPTH);

	map = new Map();
	map->populate(m_terrain->heightMap, m_terrain->treeMap, m_terrain->waterMap);

	delete m_terrain; // Don't need this anymore

	int map_w = MAP_WIDTH / 16;
	int map_h = MAP_HEIGHT / 16;	
	int map_d = MAP_DEPTH / 16;

	// TODO: Move this into Map class probably
	for (int z = 0; z < map_d; ++z)
	{
		for (int y = 0; y < map_h; ++y)
		{
			for (int x = 0; x < map_w; ++x)
			{
				if (map->chunks[map->at(x, y, z)] == nullptr) // Don't check empty chunks
				{
					continue;
				}
				else
				{
					for (int vZ = 0; vZ < 16; ++vZ)
					{
						for (int vY = 0; vY < 16; ++vY)
						{
							for (int vX = 0; vX < 16; ++vX)
							{
								int chunkIndex = map->at(x, y, z);
								int voxelIndex = map->chunks[chunkIndex]->at(vX, vY, vZ);
								char *voxel = &map->chunks[chunkIndex]->voxels[voxelIndex];

								if (*voxel == 0) // Air
								{
									continue;
								}

								if (*voxel == 1) // Grass
								{
									m_cube.instancingPositions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(x * 16 + vX, y * 16 + vY, z * 16 + vZ)));
								}
								else if (*voxel == 2) // Water
								{
									m_waterBlock.instancingPositions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(x * 16 + vX, y * 16 + vY, z * 16 + vZ)));
								}
								else if (*voxel == 3) // Tree
								{
									m_treeBlock.instancingPositions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(x * 16 + vX, y * 16 + vY, z * 16 + vZ)));
								}
								else if (*voxel == 4) // Leaf
								{
									m_leafBlock.instancingPositions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(x * 16 + vX, y * 16 + vY, z * 16 + vZ)));
								}
							}
						}
					}
				}
			}
		}		
	}

	// Confirmation that chunks are empty or contain voxels
	//for (int z = 0; z < map_d; ++z)
	//{
	//	for (int y = 0; y < map_h; ++y)
	//	{
	//		for (int x = 0; x < map_w; ++x)
	//		{
	//			if (map->chunks[x][y][z].empty)
	//			{
	//				std::cout << "Chunk [" << x << "][" << y << "][" << z << "] is empty!" << std::endl;
	//			}
	//			else
	//			{
	//				std::cout << "Chunk [" << x << "][" << y << "][" << z << "] contains voxels!" << std::endl;
	//			}
	//		}
	//	}
	//}

	// Load models
	ab::OpenGL::import("models/generic-block.obj", m_cube, "models/grass-block.png");
	ab::OpenGL::import("models/generic-block.obj", m_waterBlock, "models/water-block.png");
	ab::OpenGL::import("models/generic-block.obj", m_treeBlock, "models/tree-block.png");
	ab::OpenGL::import("models/generic-block.obj", m_leafBlock, "models/leaf-block.png");

	// Load cube map for skybox
	std::vector<std::string> f_faces
	{
		"models/alpha_island/alpha_island_rt.tga",
		"models/alpha_island/alpha_island_lf.tga",
		"models/alpha_island/alpha_island_up.tga",
		"models/alpha_island/alpha_island_dn.tga",
		"models/alpha_island/alpha_island_bk.tga",
		"models/alpha_island/alpha_island_ft.tga"
	};

	//std::vector<std::string> f_faces
	//{
	//	"models/stratosphere/stratosphere_rt.tga",
	//	"models/stratosphere/stratosphere_lf.tga",
	//	"models/stratosphere/stratosphere_up.tga",
	//	"models/stratosphere/stratosphere_dn.tga",
	//	"models/stratosphere/stratosphere_bk.tga",
	//	"models/stratosphere/stratosphere_ft.tga"
	//};

	m_cubeMapTextureID = ab::OpenGL::loadSkyBoxCubeMap(f_faces);

	// Raytracing stuff
	m_FBOtextureID = ab::OpenGL::createFBO(1280, 720);
	initialiseRaytracing();

	// Bind skybox VAO
	glGenVertexArrays(1, &m_skyboxVAO);
	glGenBuffers(1, &m_skyboxVBO);
	glBindVertexArray(m_skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

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
/// This is where the mouse events and controller events are processed.
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

		if (f_event.type == SDL_MOUSEMOTION)
		{
			// Normalised coordinates - (0, 0) is the center of the screen
			m_mousePos.x = (2.0f * f_event.motion.x) / SCREEN_WIDTH - 1.0f;
			m_mousePos.y = 1.0f - (2.0f * f_event.motion.y) / SCREEN_HEIGHT;
		}

		// Handle left and right mouse button clicks
		if (f_event.type == SDL_MOUSEBUTTONDOWN)
		{
			int x, y, z;

			if (f_event.button.button == SDL_BUTTON_LEFT) // Add voxel
			{
				m_rayDirection = m_camera->getRayFromMousePos(m_mousePos.x, m_mousePos.y);
				int index;

				if (intersectAllCubes(m_camera->getEye(), m_rayDirection, x, y, z, m_hitPoint))
				{
					m_instanceArrayUpdated = true;
				}
			}
			else if (f_event.button.button == SDL_BUTTON_RIGHT) // Delete voxel
			{
				m_rayDirection = m_camera->getRayFromMousePos(m_mousePos.x, m_mousePos.y);
				int index;

				if (intersectAllCubes(m_camera->getEye(), m_rayDirection, x, y, z, m_hitPoint))
				{
					//m_voxelOctree->set(x, y, z, false);
					m_instanceArrayUpdated = true;
				}
			}
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
	glUseProgram(m_mainShader->m_programID);
	ab::OpenGL::uniformMatrix4fv(*m_mainShader, "view", &m_camera->getView()[0][0]);
	ab::OpenGL::uniformMatrix4fv(*m_mainShader, "projection", &m_camera->getProjection()[0][0]);

	// ****************************
	// ** Dear ImGUI stuff below **
	// ****************************

	// Settings
	ImGui::Begin("SETTINGS");

	// Mouse
	ImGui::Text("MOUSE");
	ImGui::Separator();
	ImGui::BulletText("Position");
	ImGui::InputFloat("X pos", &m_mousePos.x);
	ImGui::InputFloat("Y pos", &m_mousePos.y);
	ImGui::Separator();
	ImGui::BulletText("Ray Direction");
	ImGui::InputFloat("X dir", &m_rayDirection.x);
	ImGui::InputFloat("Y dir", &m_rayDirection.y);
	ImGui::InputFloat("Z dir", &m_rayDirection.z);
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Separator();

	// Currently selected cube
	ImGui::Text("SELECTED CUBE");
	ImGui::Separator();
	ImGui::BulletText("Position");
	ImGui::InputFloat("X pos", &m_selectedCube.x);
	ImGui::InputFloat("Y pos", &m_selectedCube.y);
	ImGui::InputFloat("Z pos", &m_selectedCube.z);
	ImGui::Separator();
	ImGui::BulletText("Array Element Index");
	ImGui::Text("Index: %i", m_hitInfo.m_bi);
	ImGui::Separator();
	ImGui::BulletText("Raycast Hit");
	ImGui::InputFloat("X pos", &m_hitPoint.x);
	ImGui::InputFloat("Y pos", &m_hitPoint.y);
	ImGui::InputFloat("Z pos", &m_hitPoint.z);
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Separator();

	// Camera
	ImGui::Text("CAMERA");
	//ImGui::Separator();
	//ImGui::BulletText("Position");
	//ImGui::InputFloat("X pos", &m_cameraEye.x);
	//ImGui::InputFloat("Y pos", &m_cameraEye.y);
	//ImGui::InputFloat("Z pos", &m_cameraEye.z);

	//if (ImGui::Button("SET POSITION"))
	//{
	//	m_camera->setEye(m_cameraEye);
	//}

	//ImGui::Separator();
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
	ImGui::Checkbox("Wireframe Mode (only works with rasterization)", &m_wireframeMode);
	//ImGui::Checkbox("Raytracing On (deactivates rasterization)", &m_raytracingOn); // This breaks everything for some reason, but only if you do it during runtime - Activate at complile time and it's fine (but super slow)
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

		// If the instance array has changed then update it
		if (m_instanceArrayUpdated)
		{
			// Clear array
			if (m_cube.instancingPositions.size() > 0)
			{
				m_cube.instancingPositions.clear();
			}

			glBindBuffer(GL_ARRAY_BUFFER, m_cube.instanceBufferID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_cube.instancingPositions.size() * sizeof(glm::mat4), &m_cube.instancingPositions[0]);

			m_instanceArrayUpdated = false;
		}

		// Draw voxels
		//ab::OpenGL::uniformMatrix4fv(*m_mainShader, "model", &m_cube.matrix[0][0]);
		ab::OpenGL::uniform3f(*m_mainShader, "viewPosition", m_camera->getEye().x, m_camera->getEye().y, m_camera->getEye().z);
		ab::OpenGL::draw(m_cube, m_mainShader, "diffuseTexture");
		ab::OpenGL::draw(m_waterBlock, m_mainShader, "diffuseTexture");
		ab::OpenGL::draw(m_treeBlock, m_mainShader, "diffuseTexture");
		ab::OpenGL::draw(m_leafBlock, m_mainShader, "diffuseTexture");

		if (true)
		{
			glDepthFunc(GL_LEQUAL);

			// Activate shader
			glUseProgram(m_skyboxShader->m_programID);

			// Bind texture
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapTextureID);
			glUniform1i(glGetUniformLocation(m_skyboxShader->m_programID, "skyboxTexture"), 11);

			// Set uniforms
			glm::mat4 f_skyboxViewMatrix = glm::mat4(glm::mat3(m_camera->getView()));
			ab::OpenGL::uniformMatrix4fv(*m_skyboxShader, "view", &f_skyboxViewMatrix[0][0]);
			ab::OpenGL::uniformMatrix4fv(*m_skyboxShader, "projection", &m_camera->getProjection()[0][0]);

			// Bind VAO and draw
			glBindVertexArray(m_skyboxVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			glDepthFunc(GL_LESS);
		}
	}
	else
	{
		raytrace(); // This is a thing that does not perform well at all
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

int Game::getChunkIndex(int x, int y, int z)
{
	int map_h = MAP_HEIGHT / 16;
	int map_d = MAP_DEPTH / 16;

	return x * map_h * map_d + y * map_d + z;
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
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_voxelPositions_SSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, m_voxelPositions.size() * sizeof(glm::vec4), &m_voxelPositions[0], GL_DYNAMIC_COPY);
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
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_voxelPositions_SSBO);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, m_voxelPositions.size() * sizeof(glm::vec3), &m_voxelPositions[0], GL_READ_ONLY);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, m_voxelOctree->size() * sizeof(glm::vec3), &m_voxelOctree[0], GL_READ_ONLY);

	// Compute appropriate invocation dimension
	int f_worksizeX = ab::OpenGL::nextPowerOfTwo(1280);
	int f_worksizeY = ab::OpenGL::nextPowerOfTwo(720);

	// Invoke the compute shader
	glDispatchCompute(f_worksizeX / m_workGroupSizeX, f_worksizeY / m_workGroupSizeY, 1);

	// Reset image binding
	glBindImageTexture(0, 0, 0, false, 0, GL_READ_WRITE, GL_RGBA32F);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Render image on a quad
	renderTextureToQuad(m_FBOtextureID);
}

/// <summary>
/// Draw a quad and render a texture to it.
/// </summary>
/// <param name="t_textureID">The ID of the texture.</param>
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

/// <summary>
/// Function to check if a cube is being intersected.
/// </summary>
/// <param name="t_origin">The ray's origin point.</param>
/// <param name="t_direction">The ray's direction.</param>
/// <param name="t_cubeCenter">The center of the cube being checked.</param>
/// <returns>The near and far values.</returns>
glm::vec2 Game::intersectCube(glm::vec3 t_origin, glm::vec3 t_direction, glm::vec4 t_cubeCenter)
{
	glm::vec3 f_cubeSize(0.5, 0.5, 0.5);
	glm::vec3 f_cubeCenter = glm::vec3(t_cubeCenter.x, t_cubeCenter.y, t_cubeCenter.z);

	glm::vec3 f_min = ((f_cubeCenter - f_cubeSize) - t_origin) / t_direction;
	glm::vec3 f_max = ((f_cubeCenter + f_cubeSize) - t_origin) / t_direction;
	glm::vec3 f_t1 = glm::min(f_min, f_max);
	glm::vec3 f_t2 = glm::max(f_min, f_max);

	float f_near = std::max(std::max(f_t1.x, f_t1.y), f_t1.z);
	float f_far = std::min(std::min(f_t2.x, f_t2.y), f_t2.z);

	return glm::vec2(f_near, f_far);
}

/// <summary>
/// This function checks every cube in the world to see if a ray has intersected with it.
/// Currently, this function is horrible and inefficient but will do for testing purposes.
/// </summary>
/// <param name="t_origin">The ray's origin point.</param>
/// <param name="t_direction">The ray's direction.</param>
/// <param name="t_hitInfo">Stores the information from the cube that was intersected.</param>
/// <returns>True if an intersection has occurred.</returns>
bool Game::checkAllCubesIntersect(glm::vec3 t_origin, glm::vec3 t_direction, HitInfo &t_hitInfo)
{
	float f_smallest = m_cube.instancingPositions.size();
	bool f_found = false;

	//for (int i = 0; i < m_voxelPositions.size(); i++)
	//{
	//	glm::vec2 f_lambda = intersectCube(t_origin, t_direction, m_voxelPositions[i]);

	//	if (f_lambda.x > 0.0 && f_lambda.x < f_lambda.y && f_lambda.x < f_smallest)
	//	{
	//		t_hitInfo.m_lambda = f_lambda;
	//		t_hitInfo.m_bi = i;
	//		f_smallest = f_lambda.x;
	//		m_selectedCube = m_voxelPositions[i];
	//		f_found = true;			
	//	}
	//}

	return f_found;
}
