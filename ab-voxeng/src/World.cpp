#include "World.h"

/// <summary>
/// Constructor for the World class.
/// </summary>
World::World()
{
	std::cout << "Constructing World object" << std::endl;

	// Set world dimensions
	int x = WORLD_WIDTH / MAP_WIDTH;
	int y = WORLD_HEIGHT / MAP_HEIGHT;
	int z = WORLD_DEPTH / MAP_DEPTH;

	int size = x * y * z; // Array size

	maps.resize(size);

	// Create maps
	for (unsigned int i = 0; i < size; ++i)
	{
		maps[i] = new Map();
	}
}

/// <summary>
/// Destructor for the World class.
/// </summary>
World::~World()
{
	// Set world dimensions
	int x = WORLD_WIDTH / MAP_WIDTH;
	int y = WORLD_HEIGHT / MAP_HEIGHT;
	int z = WORLD_DEPTH / MAP_DEPTH;

	int size = x * y * z; // Array size

	// Delete chunks
	for (unsigned int i = 0; i < size; ++i)
	{
		delete maps[i];
		maps[i] = nullptr;
	}
}

/// <summary>
/// Checks maps and chunks to see if they're empty.
/// If they are, then chunk and/or map deletion occurs.
/// </summary>
void World::optimiseWorldStorage()
{
	// Find out how many maps are in the world
	int world_w = WORLD_WIDTH / MAP_WIDTH;
	int world_h = WORLD_HEIGHT / MAP_HEIGHT;
	int world_d = WORLD_DEPTH / MAP_DEPTH;

	for (int z = 0; z < world_d; ++z)
	{
		for (int y = 0; y < world_h; ++y)
		{
			for (int x = 0; x < world_w; ++x)
			{
				if (maps[Utility::at(x, y, z, world_h, world_d)] != nullptr)
				{
					maps[Utility::at(x, y, z, world_h, world_d)]->checkAllChunks();

					if (maps[Utility::at(x, y, z, world_h, world_d)]->checkIsEmpty())
					{
						delete maps[Utility::at(x, y, z, world_h, world_d)];
						maps[Utility::at(x, y, z, world_h, world_d)] = nullptr;
					}
				}
			}
		}
	}
}

/// <summary>
/// Sets a voxel to the specified type.
/// 0 = AIR / 1 = GRASS / 2 = WATER / 3 - TREE / 4 - LEAF
/// </summary>
/// <param name="x">The voxel's world position X value.</param>
/// <param name="y">The voxel's world position Y value.</param>
/// <param name="z">The voxel's world position Z value.</param>
/// <param name="type">The voxel type.</param>
void World::setVoxel(int x, int y, int z, char type)
{
	// Check if voxel is a valid position within the bounds of the world map
	if (x >= WORLD_WIDTH || x < 0 || y >= WORLD_HEIGHT || y < 0 || z >= WORLD_DEPTH || z < 0)
	{
		return;
	}

	// Get map array index
	int mapX = std::floor(x / MAP_WIDTH);
	int mapY = std::floor(y / MAP_HEIGHT);
	int mapZ = std::floor(z / MAP_DEPTH);

	// Get chunk array index
	int offsetX = std::floor(mapX * MAP_WIDTH);
	int offsetY = std::floor(mapY * MAP_HEIGHT);
	int offsetZ = std::floor(mapZ * MAP_DEPTH);

	// Get voxel array index
	int voxX = std::floor(x - offsetX);
	int voxY = std::floor(y - offsetY);
	int voxZ = std::floor(z - offsetZ);

	// Get array size
	int mapH = WORLD_HEIGHT / MAP_HEIGHT;
	int mapD = WORLD_DEPTH / MAP_DEPTH;

	// Convert 3d index to 1d index
	int index = Utility::at(mapX, mapY, mapZ, mapH, mapD);

	// If the voxel is being placed somewhere where a map doesn't
	// exist then create the map
	if (maps[index] == nullptr)
	{
		maps[index] = new Map();
	}

	// Set voxel type
	// int voxelIndex = Utility::at(voxX, voxY, voxZ, CHUNK_HEIGHT, CHUNK_DEPTH);
	maps[index]->setVoxel(voxX, voxY, voxZ, type);

	// If the map is now empty then delete it
	if (maps[index]->checkIsEmpty())
	{
		delete maps[index];
		maps[index] = nullptr;
	}
}

/// <summary>
/// Gets a specified voxel's type.
/// 0 = AIR / 1 = GRASS / 2 = WATER / 3 - TREE / 4 - LEAF
/// </summary>
/// <param name="x">The voxel's world position X value.</param>
/// <param name="y">The voxel's world position Y value.</param>
/// <param name="z">The voxel's world position Z value.</param>
/// <returns>The voxel type.</returns>
char World::getVoxel(int x, int y, int z)
{
	// Check if voxel is a valid position within the bounds of the world map
	if (x >= WORLD_WIDTH || x < 0 || y >= WORLD_HEIGHT || y < 0 || z >= WORLD_DEPTH || z < 0)
	{
		return 0;
	}

	// Get map array index
	int mapX = std::floor(x / MAP_WIDTH);
	int mapY = std::floor(y / MAP_HEIGHT);
	int mapZ = std::floor(z / MAP_DEPTH);

	// Get chunk array index
	int offsetX = std::floor(mapX * MAP_WIDTH);
	int offsetY = std::floor(mapY * MAP_HEIGHT);
	int offsetZ = std::floor(mapZ * MAP_DEPTH);

	// Get voxel array index
	int voxX = std::floor(x - offsetX);
	int voxY = std::floor(y - offsetY);
	int voxZ = std::floor(z - offsetZ);

	// Convert 3d index to 1d index
	int index = Utility::at(mapX, mapY, mapZ, MAP_HEIGHT, MAP_DEPTH);

	// If the voxel doesn't exist then return air
	if (maps[index] == nullptr)
	{
		return 0;
	}

	// Return voxel type
	return maps[index]->getVoxel(voxX, voxY, voxZ);
}

/// <summary>
/// This function populates the map with grass, water and trees.
/// 0 = AIR / 1 = GRASS / 2 = WATER / 3 - TREE / 4 - LEAF
/// </summary>
/// <param name="heightMap">An array of height map values.</param>
void World::populate(int heightMap[WORLD_WIDTH][WORLD_DEPTH], int treeMap[WORLD_WIDTH][WORLD_DEPTH], int waterMap[WORLD_WIDTH][WORLD_DEPTH])
{
	// Find out how many maps are in the world
	Indices world;
	world.x = WORLD_WIDTH / MAP_WIDTH;
	world.y = WORLD_HEIGHT / MAP_HEIGHT;
	world.z = WORLD_DEPTH / MAP_DEPTH;

	// Offset
	int offsetX = MAP_WIDTH;
	int offsetY = MAP_DEPTH;

	// Temp arrays
	auto heightMapSection = new int[MAP_WIDTH][MAP_DEPTH];
	auto treeMapSection = new int[MAP_WIDTH][MAP_DEPTH];
	auto waterMapSection = new int[MAP_WIDTH][MAP_DEPTH];

	// Divide world into map sections for population
	for (int z = 0; z < world.z; ++z)
	{
		for (int y = 0; y < world.y; ++y)
		{
			for (int x = 0; x < world.x; ++x)
			{
				if (maps[Utility::at(x, y, z, world.y, world.z)] != nullptr)
				{
					for (int mY = 0; mY < MAP_DEPTH; ++mY)
					{
						for (int mX = 0; mX < MAP_WIDTH; ++mX)
						{
							heightMapSection[mX][mY] = heightMap[mX + (x * offsetX)][mY + (z * offsetY)];
							treeMapSection[mX][mY] = treeMap[mX + (x * offsetX)][mY + (z * offsetY)];
							waterMapSection[mX][mY] = waterMap[mX + (x * offsetX)][mY + (z * offsetY)];
						}
					}

					// Populate map
					maps[Utility::at(x, y, z, world.y, world.z)]->populate(heightMapSection, treeMapSection, waterMapSection);
				}
			}
		}
	}

	placeScenery(treeMap);
}

void World::placeScenery(int treeMap[WORLD_WIDTH][WORLD_DEPTH])
{
	std::cout << "Placing scenery" << std::endl;

	for (int y = 0; y < WORLD_DEPTH; ++y)
	{
		for (int x = 0; x < WORLD_WIDTH; ++x)
		{
			if (treeMap[x][y] == 0) // 0 means no tree
			{
				continue;
			}

			// Generate slightly random values for the tree height and tree top size
			int f_treeHeight = rand() % 7 + 4;
			int f_treeTopScale = rand() % 4 + 4;

			// Creates tree trunk
			for (int i = 0; i < f_treeHeight; i++)
			{
				setVoxel(x, treeMap[x][y] + 48 + i, y, 3); // Tree
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
						setVoxel(width, height + 48, depth, 4); // Leaf
					}
				}

				modifier += 1;
			}
		}
	}

	std::cout << "Scenery successfully loaded" << std::endl;
}


