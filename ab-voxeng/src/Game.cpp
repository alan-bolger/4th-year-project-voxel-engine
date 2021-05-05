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
	std::srand(12345); // This needs to be manually entered

	SCREEN_WIDTH = t_width;
	SCREEN_HEIGHT = t_height;

	m_window = SDL_CreateWindow("Voxel-Based Map Generator [Alan Bolger]", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, t_width, t_height, SDL_WINDOW_OPENGL);

	// This is total nonsense, doesn't work properly (shows for about 2 seconds on load then vanishes)
	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	SDL_SetCursor(cursor);

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

	// Create height maps for use in map object
	m_terrain = new ab::Terrain();
	m_terrain->generate(WORLD_WIDTH, WORLD_DEPTH);

	// Create map object and populate map using height maps
	world = new World();
	world->populate(m_terrain->heightMap, m_terrain->treeMap, m_terrain->waterMap);

	delete m_terrain; // Don't need this anymore

	world->optimiseWorldStorage();	
	updateEntireMap(); // This copies all map voxels to the GPU

	// Load models
	ab::OpenGL::import("models/generic-block.obj", m_cube, "models/grass-block.png");
	ab::OpenGL::import("models/generic-block.obj", m_waterBlock, "models/water-block.png");
	ab::OpenGL::import("models/generic-block.obj", m_treeBlock, "models/tree-block.png");
	ab::OpenGL::import("models/generic-block.obj", m_leafBlock, "models/leaf-block.png");

	// Load cube map for skybox

	if (DAYTIME)
	{
		std::vector<std::string> f_faces
		{
			"models/alpha_island/alpha_island_rt.tga",
			"models/alpha_island/alpha_island_lf.tga",
			"models/alpha_island/alpha_island_up.tga",
			"models/alpha_island/alpha_island_dn.tga",
			"models/alpha_island/alpha_island_bk.tga",
			"models/alpha_island/alpha_island_ft.tga"
		};

		m_cubeMapTextureID = ab::OpenGL::loadSkyBoxCubeMap(f_faces);
	}
	else
	{
		std::vector<std::string> f_faces
		{
			"models/stratosphere/stratosphere_rt.tga",
			"models/stratosphere/stratosphere_lf.tga",
			"models/stratosphere/stratosphere_up.tga",
			"models/stratosphere/stratosphere_dn.tga",
			"models/stratosphere/stratosphere_bk.tga",
			"models/stratosphere/stratosphere_ft.tga"
		};

		m_cubeMapTextureID = ab::OpenGL::loadSkyBoxCubeMap(f_faces);
	}	

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
			//if (f_event.button.button == SDL_BUTTON_LEFT) // Add voxel
			//{
			//	m_rayDirection = m_camera->getRayFromMousePos(m_mousePos.x, m_mousePos.y);

			//	if (checkForVoxelIntersections(m_camera->getEye(), m_rayDirection, m_hitPoint))
			//	{
			//		world->setVoxel((int)m_hitInfo.center.x, (int)m_hitInfo.center.y, (int)m_hitInfo.center.z, 0);
			//		updateEntireMap();
			//	}
			//}
			//else if (f_event.button.button == SDL_BUTTON_RIGHT) // Delete voxel
			//{
			//	m_rayDirection = m_camera->getRayFromMousePos(m_mousePos.x, m_mousePos.y);

			//	if (checkForVoxelIntersections(m_camera->getEye(), m_rayDirection, m_hitPoint))
			//	{
			//		map->voxel((int)m_hitInfo.center.x, (int)m_hitInfo.center.y, (int)m_hitInfo.center.z, 0);
			//		updateEntireMap();
			//	}
			//}
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
		// This only gets done if a voxel is added or removed
		// TODO: This really needs to be split up somehow, updating the entire array is a bit mad
		if (m_instanceArrayUpdated)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_cube.instanceBufferID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_cube.instancingPositions.size() * sizeof(glm::mat4), &m_cube.instancingPositions[0]);

			m_instanceArrayUpdated = false;
		}

		// Send camera position to shader
		ab::OpenGL::uniform3f(*m_mainShader, "viewPosition", m_camera->getEye().x, m_camera->getEye().y, m_camera->getEye().z);

		// Draw cubes using instancing
		ab::OpenGL::draw(m_cube, m_mainShader, "diffuseTexture");
		ab::OpenGL::draw(m_waterBlock, m_mainShader, "diffuseTexture");
		ab::OpenGL::draw(m_treeBlock, m_mainShader, "diffuseTexture");
		ab::OpenGL::draw(m_leafBlock, m_mainShader, "diffuseTexture");

		if (true) // TODO: Add this to ImGUI as an option
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

/// <summary>
/// Gets the chunk array index using a world position.
/// </summary>
/// <param name="x">X</param>
/// <param name="y">Y</param>
/// <param name="z">Z</param>
/// <returns>An integer of great value.</returns>
int Game::getChunkIndex(int x, int y, int z)
{
	int map_h = 16;
	int map_d = 16;

	Indices indices;

	indices.x = std::floor(x / 16);
	indices.y = std::floor(y / 16);
	indices.z = std::floor(z / 16);

	return indices.x * map_h * map_d + indices.y * map_d + indices.z;
}

/// <summary>
/// Adds voxel positions ALL arrays that are to be sent to the GPU.
/// </summary>
void Game::updateEntireMap()
{
	// Find out how many maps are in the world
	int world_w = WORLD_WIDTH / MAP_WIDTH;
	int world_h = WORLD_HEIGHT / MAP_HEIGHT;
	int world_d = WORLD_DEPTH / MAP_DEPTH;

	if (m_cube.instancingPositions.size() > 0)
	{
		m_cube.instancingPositions.clear();
	}

	if (m_waterBlock.instancingPositions.size() > 0)
	{
		m_waterBlock.instancingPositions.clear();
	}

	if (m_treeBlock.instancingPositions.size() > 0)
	{
		m_treeBlock.instancingPositions.clear();
	}

	if (m_leafBlock.instancingPositions.size() > 0)
	{
		m_leafBlock.instancingPositions.clear();
	}

	for (int wZ = 0; wZ < world_d; ++wZ)
	{
		for (int wY = 0; wY < world_h; ++wY)
		{
			for (int wX = 0; wX < world_w; ++wX)
			{
				if (world->maps[Utility::at(wX, wY, wZ, world_h, world_d)] == nullptr) // Don't check empty maps
				{
					continue;
				}
				else
				{
					for (int mZ = 0; mZ < MAP_WIDTH / CHUNK_WIDTH; ++mZ)
					{
						for (int mY = 0; mY < MAP_HEIGHT / CHUNK_HEIGHT; ++mY)
						{
							for (int mX = 0; mX < MAP_DEPTH / CHUNK_DEPTH; ++mX)
							{
								if (world->maps[Utility::at(wX, wY, wZ, world_h, world_d)]->chunks[Utility::at(mX, mY, mZ, MAP_HEIGHT / CHUNK_HEIGHT, MAP_DEPTH / CHUNK_DEPTH)] == nullptr) // Don't check empty chunks
								{
									continue;
								}
								else
								{
									for (int cZ = 0; cZ < CHUNK_DEPTH; ++cZ)
									{
										for (int cY = 0; cY < CHUNK_HEIGHT; ++cY)
										{
											for (int cX = 0; cX < CHUNK_WIDTH; ++cX)
											{
												char *voxel = &world->maps[Utility::at(wX, wY, wZ, world_h, world_d)]->chunks[Utility::at(mX, mY, mZ, MAP_HEIGHT / CHUNK_HEIGHT, MAP_DEPTH / CHUNK_DEPTH)]->voxels[Utility::at(cX, cY, cZ, CHUNK_HEIGHT, CHUNK_DEPTH)];

												if (*voxel == 0) // Air
												{
													continue; // Break from current loop if voxel is air
												}

												if (*voxel == 1) // Grass
												{
													if (m_raytracingOn)
													{
														m_voxelPositions.push_back(glm::vec4((wX * MAP_WIDTH) + (mX * CHUNK_WIDTH) + cX, (wY * MAP_HEIGHT) + (mY * CHUNK_HEIGHT) + cY, (wZ * MAP_DEPTH) + (mZ * CHUNK_DEPTH) + cZ, 1));
													}				
													else
													{
														m_cube.instancingPositions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3((wX * MAP_WIDTH) + (mX * CHUNK_WIDTH) + cX, (wY * MAP_HEIGHT) + (mY * CHUNK_HEIGHT) + cY, (wZ * MAP_DEPTH) + (mZ * CHUNK_DEPTH) + cZ)));
													}
												}
												else if (*voxel == 2) // Water
												{
													if (m_raytracingOn)
													{
														m_voxelPositions.push_back(glm::vec4((wX * MAP_WIDTH) + (mX * CHUNK_WIDTH) + cX, (wY * MAP_HEIGHT) + (mY * CHUNK_HEIGHT) + cY, (wZ * MAP_DEPTH) + (mZ * CHUNK_DEPTH) + cZ, 1));
													}
													else
													{
														m_waterBlock.instancingPositions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3((wX * MAP_WIDTH) + (mX * CHUNK_WIDTH) + cX, (wY * MAP_HEIGHT) + (mY * CHUNK_HEIGHT) + cY, (wZ * MAP_DEPTH) + (mZ * CHUNK_DEPTH) + cZ)));
													}
												}
												else if (*voxel == 3) // Tree
												{									
													if (m_raytracingOn)
													{
														m_voxelPositions.push_back(glm::vec4((wX * MAP_WIDTH) + (mX * CHUNK_WIDTH) + cX, (wY * MAP_HEIGHT) + (mY * CHUNK_HEIGHT) + cY, (wZ * MAP_DEPTH) + (mZ * CHUNK_DEPTH) + cZ, 1));
													}
													else
													{
														m_treeBlock.instancingPositions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3((wX * MAP_WIDTH) + (mX * CHUNK_WIDTH) + cX, (wY * MAP_HEIGHT) + (mY * CHUNK_HEIGHT) + cY, (wZ * MAP_DEPTH) + (mZ * CHUNK_DEPTH) + cZ)));
													}
												}
												else if (*voxel == 4) // Leaf
												{												
													if (m_raytracingOn)
													{
														m_voxelPositions.push_back(glm::vec4((wX * MAP_WIDTH) + (mX * CHUNK_WIDTH) + cX, (wY * MAP_HEIGHT) + (mY * CHUNK_HEIGHT) + cY, (wZ * MAP_DEPTH) + (mZ * CHUNK_DEPTH) + cZ, 1));
													}
													else
													{
														m_leafBlock.instancingPositions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3((wX * MAP_WIDTH) + (mX * CHUNK_WIDTH) + cX, (wY * MAP_HEIGHT) + (mY * CHUNK_HEIGHT) + cY, (wZ * MAP_DEPTH) + (mZ * CHUNK_DEPTH) + cZ)));
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	m_instanceArrayUpdated = true;
}

/// <summary>
/// Gets the chunk [x, y, z] positions in the array from a world position.
/// </summary>
/// <param name="x">X</param>
/// <param name="y">Y</param>
/// <param name="z">Z</param>
/// <returns>An index value.</returns>
Indices Game::getChunkXYZ(int x, int y, int z)
{
	Indices indices;

	indices.x = std::floor(x / 16);
	indices.y = std::floor(y / 16);
	indices.z = std::floor(z / 16);

	return indices;
}

/// <summary>
/// Used for mouse interaction with voxels.
/// </summary>
/// <param name="t_origin">The origin point (the current mouse position).</param>
/// <param name="t_direction">The ray direction.</param>
/// <param name="t_hitPoint">The coordinates of the voxel that was hit with the ray.</param>
/// <returns>True if an intersection was found.</returns>
bool Game::checkForVoxelIntersections(glm::vec3 t_origin, glm::vec3 t_direction, glm::vec3& t_hitPoint)
{
	// Only check neighbouring chunks for intersections
	glm::vec3 worldPosition = m_camera->getEye();

	// This is the chunk to start checking at
	Indices chunkStart = getChunkXYZ(worldPosition.x, worldPosition.y, worldPosition.z);

	// Set chunk starting point [x, y, z]
	chunkStart.x--;
	chunkStart.y--;
	chunkStart.z--;

	// Get chunk map array dimensions
	int map_w = MAP_WIDTH / CHUNK_WIDTH;
	int map_d = MAP_DEPTH / CHUNK_DEPTH;
	
	// Check all the immediate chunks around you (including the one you're in)
	//for (int z = 0; z < 3; ++z)
	//{
	//	for (int y = 0; y < 3; ++y)
	//	{
	//		for (int x = 0; x < 3; ++x)
	//		{
	//			int index = Utility::at(chunkStart.x + x, chunkStart.y + y, chunkStart.z + z, map_w, map_d);

	//			if (index < 0 || index > map->chunks.size())
	//			{
	//				continue; // If chunk is out of bounds then go to next iteration
	//			}

	//			// If chunk is a null pointer then the chunk is air (and also doesn't exist)
	//			if (map->chunks[index] != nullptr)
	//			{
	//				Indices chunkIndex = { chunkStart.x + x, chunkStart.y + y, chunkStart.z + z };

	//				if (map->chunks[index]->checkAllCubesIntersect(t_origin, t_direction, chunkIndex, m_hitInfo))
	//				{
	//					return true;
	//				}
	//			}
	//		}
	//	}
	//}

	return false;
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
/// Perform ray tracing.
/// Ray Tracing is experimental and does not work correctly.
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
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_voxelPositions.size() * sizeof(glm::vec3), &m_voxelPositions[0], GL_READ_ONLY);

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


