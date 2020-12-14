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
	glm::vec3 first;
	glm::vec3 second;
};

namespace ab
{
	class Terrain
	{
	public:
		Terrain();
		~Terrain();
		void generate(int t_width, int t_height);

	private:
		Noise *m_noise;
		Box m_boxes[4][4];
		static const int MAP_WIDTH = 16;
		static const int MAP_HEIGHT = 16;

		void initialise();
	};
}

#endif // !TERRAIN_H
