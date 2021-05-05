// *******************************************
// * Game.h and Game.cpp - Alan Bolger, 2020 *
// *******************************************

#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <vector>

#include "Globals.h"
#include "glew/glew.h"
#include "glew/wglew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "Shader.h"
#include "OpenGL.h"
#include "Camera.h"
#include "XboxOneController.h"
#include "Terrain.h"
#include "Debug.h"
#include "World.h"
#include "Timer.h" // Thanks to Paul O' Callaghan for this!

class Game
{
public:
	Game(int t_width, int t_height);
	~Game();
	void start();

private:
	HitInfo m_hitInfo; // Used for ray casting

	SDL_Window *m_window;
	SDL_GLContext m_glContext;
	double m_frameRate;
	double m_frameMs;
	bool m_looping;
	ab::XboxOneController *m_controller;
	ab::Camera *m_camera;
	ab::Shader *m_mainShader;
	ab::Shader *m_renderQuadShader;
	ab::Shader *m_computeShader;
	ab::Shader *m_skyboxShader;
	ab::Terrain *m_terrain;
	SDL_Cursor *cursor;
	ab::Model m_cube;
	ab::Model m_waterBlock;
	ab::Model m_treeBlock;
	ab::Model m_leafBlock;
	glm::vec3 m_directionalLightDirection = glm::vec3(1, -5, 1);
	float m_directionalLightAmbient[3] = { 1.f, 1.f, 1.f };
	float m_directionalLightDiffuse[3] = { 1.f, 1.f, 1.f };
	bool m_wireframeMode = false;
	glm::vec2 m_mousePos;
	glm::vec3 m_rayDirection;
	glm::vec3 m_cameraEye;
	glm::vec4 m_selectedCube;
	glm::vec3 m_hitPoint;
	int m_comboType = 0;
	bool m_raytracingOn = false;
	bool m_instanceArrayUpdated = false;
	World *world;

	// Quad for render to texture
	GLuint m_quadVertexArrayObjectID;
	GLuint m_quadVertexBufferObjectID;

	// Used for skybox
	GLuint m_cubeMapTextureID;
	GLuint m_skyboxVAO;
	GLuint m_skyboxVBO;
	float skyboxVertices[108] = // These vertices are used for the skybox cubemap
	{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// These vertices are used to draw a quad
	// The quad is used for rendering to a texture
	GLfloat m_quadVertices[18] =
	{
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	// Compute shader stuff
	GLint m_workGroupSizeX;
	GLint m_workGroupSizeY;
	GLuint m_frameBufferID;
	GLuint m_FBOtextureID;
	glm::vec3 m_eyeRay;
	GLuint m_voxelPositions_SSBO;
	std::vector<glm::vec4> m_voxelPositions;

	void initialise();
	void processEvents();
	void update(double t_deltaTime);
	void draw();
	int getChunkIndex(int x, int y, int z);
	void updateEntireMap();
	void initialiseRaytracing();
	void raytrace();
	void renderTextureToQuad(GLuint &t_textureID);
	Indices getChunkXYZ(int x, int y, int z);
	bool checkForVoxelIntersections(glm::vec3 t_origin, glm::vec3 t_direction, glm::vec3& t_hitPoint);
};

#endif // !GAME_H