// *****************************************
// * Map.h and Map.cpp - Alan Bolger, 2020 *
// *****************************************

#ifndef MAP_H
#define MAP_H

#include "Chunk.h"

class Map
{
public:
	Map()
	{
		int X = 16;
		int Y = 16;
		int Z = 16;

		chunks = new Chunk**[2048];

		for (int i = 0; i < X; i++)
		{
			chunks[i] = new Chunk*[Y];

			for (int j = 0; j < Y; j++) 
			{
				chunks[i][j] = new Chunk[Z];
			}
		}
	};

	~Map()
	{
		int X = 16;
		int Y = 16;
		int Z = 16;

		for (int i = 0; i < X; i++)
		{
			for (int j = 0; j < Y; j++) 
			{
				delete[] chunks[i][j];
			}

			delete[] chunks[i];
		}

		delete[] chunks;
	};

	/// <summary>
	/// Set a voxel to visible or not visible.
	/// </summary>
	/// <param name="x">The voxel's world position x value.</param>
	/// <param name="y">The voxel's world position y value.</param>
	/// <param name="z">The voxel's world position z value.</param>
	/// <param name="visible">True or false. You decide.</param>
	void voxel(int x, int y, int z, bool visible)
	{
		// Get chunk
		int chunkX = x / 16;
		int chunkY = y / 16;
		int chunkZ = z / 16;

		// Get bit
		int bitX = chunkX / 4;
		int bitY = chunkY / 4;
		int bitZ = chunkZ / 4;

		// Get voxel
		int voxX = bitX / 4;
		int voxY = bitY / 4;
		int voxZ = bitZ / 4;

		chunks[chunkX][chunkY][chunkZ].bits[bitX][bitY][bitZ].voxels[voxX][voxY][voxZ].exists = visible;
	}

	/// <summary>
	/// Finds out if a voxel is visible or not visible.
	/// </summary>
	/// <param name="x">The voxel's world position x value.</param>
	/// <param name="y">The voxel's world position y value.</param>
	/// <param name="z">The voxel's world position z value.</param>
	/// <param name="visible">True or false. You decide.</param>
	bool voxel(int x, int y, int z)
	{
		// Get chunk
		int chunkX = std::floor(x / 16);
		int chunkY = std::floor(y / 16);
		int chunkZ = std::floor(z / 16);

		// Get bit
		int bitX = chunkX / 4;
		int bitY = chunkY / 4;
		int bitZ = chunkZ / 4;

		// Get voxel
		int voxX = bitX / 4 / 4;
		int voxY = bitY / 4 / 4;
		int voxZ = bitZ / 4 / 4;

		return chunks[chunkX][chunkY][chunkZ].bits[bitX][bitY][bitZ].voxels[voxX][voxY][voxZ].exists;
	}

	/// <summary>
	/// Populates the map with voxels.
	/// </summary>
	/// <param name="heightMap">An array of height map values.</param>
	void populate(int heightMap[256][256])
	{
		for (int y = 0; y < 256; ++y)
		{
			for (int x = 0; x < 256; ++x)
			{
				voxel(x, heightMap[x][y], y, true);
			}
		}
	};

private:
	Chunk ***chunks;
};

#endif // !MAP_H

