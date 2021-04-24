// *****************************************
// * Map.h and Map.cpp - Alan Bolger, 2020 *
// *****************************************

#ifndef MAP_H
#define MAP_H

#include "Chunk.h"
#include "Globals.h"

class Map
{
public:
	Map()
	{
		int X = MAP_WIDTH / 16;
		int Y = MAP_HEIGHT / 16;
		int Z = MAP_DEPTH / 16;

		chunks = new Chunk**[4096];

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
		int X = MAP_WIDTH / 16;
		int Y = MAP_HEIGHT / 16;
		int Z = MAP_DEPTH / 16;

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
	/// Change this to a colour value
	/// </summary>
	/// <param name="x">The voxel's world position x value.</param>
	/// <param name="y">The voxel's world position y value.</param>
	/// <param name="z">The voxel's world position z value.</param>
	/// <param name="visible">True or false. You decide.</param>
	void voxel(int x, int y, int z, Voxel voxelType)
	{
		if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0 || z >= MAP_DEPTH || z < 0)
		{
			return;
		}

		// Get chunk
		int chunkX = std::floor(x / 16);
		int chunkY = std::floor(y / 16);
		int chunkZ = std::floor(z / 16);

		// Get voxel
		int voxX = std::floor(x % 16);
		int voxY = std::floor(y % 16);
		int voxZ = std::floor(z % 16);

		chunks[chunkX][chunkY][chunkZ].voxels[voxX][voxY][voxZ] = voxelType;
	}

	/// <summary>
	/// Finds out if a voxel is visible or not visible.
	/// </summary>
	/// <param name="x">The voxel's world position x value.</param>
	/// <param name="y">The voxel's world position y value.</param>
	/// <param name="z">The voxel's world position z value.</param>
	/// <param name="visible">True or false. You decide.</param>
	Voxel voxel(int x, int y, int z)
	{
		if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0 || z >= MAP_DEPTH || z < 0)
		{
			return Voxel::AIR;
		}

		// Get chunk
		int chunkX = std::floor(x / 16);
		int chunkY = std::floor(y / 16);
		int chunkZ = std::floor(z / 16);

		// Get voxel
		int voxX = std::floor(x % 16);
		int voxY = std::floor(y % 16);
		int voxZ = std::floor(z % 16);

		return chunks[chunkX][chunkY][chunkZ].voxels[voxX][voxY][voxZ];
	}

	/// <summary>
	/// Populates the map with voxels.
	/// </summary>
	/// <param name="heightMap">An array of height map values.</param>
	void populate(int heightMap[MAP_WIDTH][MAP_DEPTH], int treeMap[MAP_WIDTH][MAP_DEPTH], int waterMap[MAP_WIDTH][MAP_DEPTH])
	{
		for (int y = 0; y < MAP_DEPTH; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				voxel(x, heightMap[x][y], y, Voxel::GRASS);
			}
		}

		for (int y = 0; y < MAP_DEPTH; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				voxel(x, waterMap[x][y], y, Voxel::WATER);
			}
		}

		placeScenery(treeMap);
		checkAllChunks();
	};

	/// <summary>
	/// Populates the map with voxels.
	/// </summary>
	/// <param name="heightMap">An array of height map values.</param>
	void placeScenery(int treeMap[MAP_WIDTH][MAP_DEPTH])
	{
		for (int y = 0; y < MAP_DEPTH; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				if (treeMap[x][y] == 0) // 0 means no tree
				{
					continue;
				}

				int f_treeHeight = rand() % 7 + 4; // Height will be between 6 to 9 units for now
				int f_treeTopScale = rand() % 4 + 4; // Size will be between 4 to 6 units for now

				// Creates a tree trunk
				for (int i = 0; i < f_treeHeight; i++)
				{
					voxel(x, treeMap[x][y] + i, y, Voxel::TREE);
				}

				int yBegin = treeMap[x][y] + f_treeHeight;

				// Creates a tree top
				for (int fz = y - f_treeTopScale; fz < y + f_treeTopScale + 1; ++fz)
				{
					for (int fy = yBegin; fy < yBegin + f_treeTopScale + 1; ++fy)
					{
						for (int fx = x - f_treeTopScale; fx < x + f_treeTopScale + 1; ++fx)
						{
							voxel(fx, fy, fz, Voxel::LEAF);
						}
					}
				}
			}
		}

		checkAllChunks();
	};

	void checkAllChunks()
	{
		int map_w = MAP_WIDTH / 16;
		int map_h = MAP_HEIGHT / 16;
		int map_d = MAP_DEPTH / 16;

		for (int z = 0; z < map_d; ++z)
		{
			for (int y = 0; y < map_h; ++y)
			{
				for (int x = 0; x < map_w; ++x)
				{
					chunks[x][y][z].check();
				}
			}
		}
	}

	Chunk ***chunks;
};

#endif // !MAP_H