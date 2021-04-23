// *************************************************
// * Terrain.h and Terrain.cpp - Alan Bolger, 2020 *
// *************************************************

#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Noise.h"
#include "Globals.h"

#include <math.h>
#include <vector>
#include <random>

struct Box
{
	glm::vec3 center;
	glm::vec3 colour;
};

namespace ab
{
	class Terrain
	{
	public:
		//static const int MAP_WIDTH = 256;
		//static const int MAP_HEIGHT = 256;
		// Box m_boxes[MAP_HEIGHT][MAP_WIDTH];
		int heightMap[MAP_WIDTH][MAP_DEPTH];
		int waterMap[MAP_WIDTH][MAP_DEPTH];

		Terrain();
		~Terrain();
		void generate(int t_width, int t_height);

	private:
		Noise *m_noise;

		void initialise();
	};
}

#endif // !TERRAIN_H
