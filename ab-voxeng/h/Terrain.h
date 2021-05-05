// ********************************************************************************
// * Terrain.h and Terrain.cpp contains code based on the tutorial				  *
// * 'Making maps with noise functions' by Red Blob Games, 2015.				  *
// * https://www.redblobgames.com/maps/terrain-from-noise/						  *
// ********************************************************************************

#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Noise.h"
#include "Globals.h"

#include <math.h>
#include <vector>
#include <random>

namespace ab
{
	class Terrain
	{
	public:
		int heightMap[WORLD_WIDTH][WORLD_DEPTH];
		int waterMap[WORLD_WIDTH][WORLD_DEPTH];
		int treeMap[WORLD_WIDTH][WORLD_DEPTH];

		Terrain();
		~Terrain();
		void generate(int t_width, int t_height);

	private:
		Noise *m_noise;

		void initialise();
	};
}

#endif // !TERRAIN_H
