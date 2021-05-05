// *********************************
// * Globals.h - Alan Bolger, 2020 *
// *********************************

#ifndef GLOBALS_H
#define GLOBALS_H

#include "glm/glm.hpp"

// Values below should ALWAYS be power of 2 numbers,
// otherwise things may break quite horribly.....
// Also, keep the world height and the map height the same

// Screen dimensions
static unsigned int SCREEN_WIDTH;
static unsigned int SCREEN_HEIGHT;

// World data
static const int WORLD_WIDTH = 1024;
static const int WORLD_HEIGHT = 128;
static const int WORLD_DEPTH = 1024;

// Map generation data
static const int MAP_WIDTH = 256;
static const int MAP_HEIGHT = 128;
static const int MAP_DEPTH = 256;
static const int WATER_HEIGHT = 1;
static const float EXP = 4.0f; // This adjusts hills and valleys
static const bool DAYTIME = true; // Set this to false for night

// This is the size of a chunk in voxels
static const int CHUNK_WIDTH = 16;
static const int CHUNK_HEIGHT = 16;
static const int CHUNK_DEPTH = 16;


// Simple struct for storing array [x, y, z] index values
struct Indices
{
	int x;
	int y;
	int z;
};

// A small collection of useful functions for various things and stuff and whatnot
class Utility
{
public:

	/// <summary>
	/// Use this to convert a 3D array index value into a 1D array index value.
	/// </summary>
	/// <param name="x">The X index value of the array.</param>
	/// <param name="y">The Y index value of the array.</param>
	/// <param name="z">The Z index value of the array.</param>
	/// <param name="arrH">The height of the array.</param>
	/// <param name="arrD">The depth of the array.</param>
	/// <returns>A 1d array index value.</returns>
	static int at(int x, int y, int z, int arrH, int arrD)
	{
		return x * arrH * arrD + y * arrD + z;
	}

	/// <summary>
	/// Use this to convert a 1D array index value into a 3D array index value.
	/// </summary>
	/// <param name="i">The 1D index value.</param>
	/// <param name="arrH">The height of the array.</param>
	/// <param name="arrD">The depth of the array.</param>
	/// <returns>A 3d array index value.</returns>
	static Indices at(int i, int arrH, int arrD)
	{
		Indices indices;
		indices.x = i / (arrH * arrD);
		indices.y = (i - indices.x * arrH * arrD) / arrD;
		indices.z = i - indices.x * arrH * arrD - indices.y * arrD;

		return indices;
	}
};

// Used to store ray casting hit data
struct HitInfo
{
	int bi;
	glm::vec2 lambda;
	glm::vec3 intersectPoint;
	glm::vec3 center;
};

#endif // !GLOBALS_H

