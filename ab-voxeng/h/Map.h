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
		int x = MAP_WIDTH / 16;
		int y = MAP_HEIGHT / 16;
		int z = MAP_DEPTH / 16;
		int size = x * y * z;

		chunks.resize(size);

		for (unsigned int i = 0; i < size; ++i)
		{
			chunks[i] = new Chunk();
		}
	}

	~Map()
	{
		int x = MAP_WIDTH / 16;
		int y = MAP_HEIGHT / 16;
		int z = MAP_DEPTH / 16;
		int size = x * y * z;

		for (unsigned int i = 0; i < size; ++i)
		{
			delete chunks[i];
		}
	}

	/// <summary>
	/// Sets info on a specific voxel.
	/// </summary>
	/// <param name="x">The voxel's world position x value.</param>
	/// <param name="y">The voxel's world position y value.</param>
	/// <param name="z">The voxel's world position z value.</param>
	void voxel(int x, int y, int z, char type)
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

		int index = this->at(chunkX, chunkY, chunkZ);
		chunks[index]->voxels[chunks[index]->at(voxX, voxY, voxZ)] = type;
	}

	/// <summary>
	/// Gets info on a specific voxel.
	/// </summary>
	/// <param name="x">The voxel's world position x value.</param>
	/// <param name="y">The voxel's world position y value.</param>
	/// <param name="z">The voxel's world position z value.</param>
	/// <returns>The voxel type at the given position.</returns>
	char voxel(int x, int y, int z)
	{
		if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0 || z >= MAP_DEPTH || z < 0)
		{
			return 0;
		}
		
		// Get chunk
		int chunkX = std::floor(x / 16);
		int chunkY = std::floor(y / 16);
		int chunkZ = std::floor(z / 16);

		// Get voxel
		int voxX = std::floor(x % 16);
		int voxY = std::floor(y % 16);
		int voxZ = std::floor(z % 16);

		int index = this->at(chunkX, chunkY, chunkZ);
		return chunks[index]->voxels[chunks[index]->at(voxX, voxY, voxZ)];
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
				voxel(x, heightMap[x][y], y, 1); // Grass
				voxel(x, waterMap[x][y], y, 2);// Water
			}
		}

		placeScenery(treeMap); // Trees
		checkAllChunks(); // Delete empty chunks
	}

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

				int f_treeHeight = rand() % 7 + 4;
				int f_treeTopScale = rand() % 4 + 4;

				// Creates a tree trunk of random height
				for (int i = 0; i < f_treeHeight; i++)
				{
					voxel(x, treeMap[x][y] + i, y, 3);
				}

				int yBegin = treeMap[x][y] + f_treeHeight;

				// Creates a pointy tree top of a random size
				int modifier = 0;

				for (int height = yBegin; height < yBegin + f_treeTopScale + 1; ++height)
				{
					for (int depth = y - f_treeTopScale + modifier; depth < y + f_treeTopScale - modifier + 1; ++depth)
					{
						for (int width = x - f_treeTopScale + modifier; width < x + f_treeTopScale - modifier + 1; ++width)
						{
							voxel(width, height, depth, 4);
						}
					}

					modifier += 1;
				}				
			}
		}
	}

	/// <summary>
	/// Checks all of the chunks in a map and deletes them if they contain only air.
	/// </summary>
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
					// If there is a null pointer at [x, y, z] then that chunk
					// is empty/doesn't exist and doesn't need to be checked
					if (chunks[at(x, y, z)] == nullptr)
					{
						continue;
					}
					// If the pointer is not null then check the chunk it's pointing to
					else if (chunks[at(x, y, z)]->checkIsEmpty())
					{
						delete chunks[at(x, y, z)]; // Delete the empty chunk
						chunks[at(x, y, z)] = nullptr; // Make pointer null
					}
				}
			}
		}
	}

	/// <summary>
	/// Use this to convert a 3D array index value into a 1D array index value.
	/// This is specifically used to get the index value from an array of dimensions 16 x 16 x 16.
	/// Only use this with chunks.
	/// </summary>
	/// <param name="x">The X value.</param>
	/// <param name="y">The Y value.</param>
	/// <param name="z">The Z value.</param>
	/// <returns>A 1D array index value.</returns>
	int at(int x, int y, int z)
	{
		int map_h = MAP_HEIGHT / 16;
		int map_d = MAP_DEPTH / 16;

		return x * map_h * map_d + y * map_d + z;
	}

	/// <summary>
	/// Use this to convert a 1D array index value into a 3D array index value.
	/// This is specifically used to get the index value from an array of dimensions 16 x 16 x 16.
	/// Only use this with chunks.
	/// </summary>
	/// <param name="i">The 1D index value.</param>
	/// <returns>A 3D array index value.</returns>
	Indices at(int i)
	{
		int map_h = MAP_HEIGHT / 16;
		int map_d = MAP_DEPTH / 16;

		Indices indices;
		indices.x = i / (map_h * map_d);
		indices.y = (i - indices.x * map_h * map_d) / map_d;
		indices.z = i - indices.x * map_h * map_d - indices.y * map_d;

		return indices;
	}

	std::vector<Chunk*> chunks;
};

#endif // !MAP_H