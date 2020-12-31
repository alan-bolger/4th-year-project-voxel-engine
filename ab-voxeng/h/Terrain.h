#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Noise.h"

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
		static const int MAP_WIDTH = 1024;
		static const int MAP_HEIGHT = 1024;
		Box m_boxes[MAP_HEIGHT][MAP_WIDTH];

		Terrain();
		~Terrain();
		void generate(int t_width, int t_height);

	private:
		Noise *m_noise;

		void initialise();
	};
}

#endif // !TERRAIN_H
