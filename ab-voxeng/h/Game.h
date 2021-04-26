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
#include "Map.h"

class Ray
{
public:
	// TODO: Consider putting this into a struct possibly
	Ray(const glm::vec3 &orig, const glm::vec3 &dir) : orig(orig), dir(dir)
	{
		invdir = 1.0f / dir;
		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);
	}

	glm::vec3 orig, dir; // ray orig and dir 
	glm::vec3 invdir;
	int sign[3];
};

class Game
{
public:
	Game(int t_width, int t_height);
	~Game();
	void start();

private:
	// TODO: Move this struct somewhere else
	struct HitInfo
	{
		int m_bi;
		glm::vec2 m_lambda;
		glm::vec3 m_center;
	}
	m_hitInfo;

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
	Map *map;
	Map *water;
	Map *trees;
	//Octree<bool> *m_voxelOctree;

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
	void getVoxelIndex(int chunkIndex, int x, int y, int z);
	void initialiseRaytracing();
	void raytrace();
	void renderTextureToQuad(GLuint &t_textureID);
	glm::vec2 intersectCube(glm::vec3 t_origin, glm::vec3 t_direction, glm::vec4 t_cubeCenter);
	bool checkAllCubesIntersect(glm::vec3 t_origin, glm::vec3 t_direction, HitInfo &t_hitInfo);

	bool intersect(const Ray &r, float &t, int x, int y, int z) const
	{
		glm::vec3 bounds[2];
		glm::vec3 f_cubeSize(0.5, 0.5, 0.5);
		glm::vec3 f_cubeCenter = glm::vec3(x, y, z);

		bounds[0] = f_cubeCenter - f_cubeSize;
		bounds[1] = f_cubeCenter + f_cubeSize;

		float tmin, tmax, tymin, tymax, tzmin, tzmax;

		tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
		tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
		tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
		tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;

		tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
		tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;

		t = tmin;

		if (t < 0) {
			t = tmax;
			if (t < 0) return false;
		}

		return true;
	}
	
	bool intersectAllCubes(glm::vec3 t_origin, glm::vec3 t_direction, int &t_x, int &t_y, int &t_z, glm::vec3 &t_hitPoint)
	{
		Ray ray(t_origin, t_direction);
		float t;

		//for (int i = 0; i < m_voxelPositions.size(); i++)
		//{
		//	if (intersect(ray, t, m_voxelPositions[i]))
		//	{
		//		t_index = i;
		//		m_hitPoint = ray.orig + ray.dir * t;
		//		return true;
		//	}
		//}

		//for (int z = 0; z < 256; ++z)
		//{
		//	for (int y = 0; y < 256; ++y)
		//	{
		//		for (int x = 0; x < 256; ++x)
		//		{
		//			if (m_voxelPositions->at(x, y, z) == false)
		//			{
		//				break;
		//			}

		//			if (intersect(ray, t, x, y, z))
		//			{
		//				m_hitPoint = ray.orig + ray.dir * t;
		//				t_x = x;
		//				t_y = y;
		//				t_z = z;

		//				return true;
		//			}
		//		}
		//	}
		//}

		return false;
	}
};

#endif // !GAME_H