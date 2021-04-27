// *********************************
// * Globals.h - Alan Bolger, 2020 *
// *********************************

#ifndef GLOBALS_H
#define GLOBALS_H

#include "glm/glm.hpp"

static unsigned int SCREEN_WIDTH;
static unsigned int SCREEN_HEIGHT;

static const int MAP_WIDTH = 1024;
static const int MAP_HEIGHT = 128;
static const int MAP_DEPTH = 1024;
static const int WATER_HEIGHT = 1;

struct Indices
{
	int x;
	int y;
	int z;
};

struct HitInfo
{
	int bi;
	glm::vec2 lambda;
	glm::vec3 intersectPoint;
	glm::vec3 center;
};

#endif // !GLOBALS_H

