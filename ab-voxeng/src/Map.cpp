#include "Map.h"

Map::Map()
{
	// Find out how many chunks will fit in the map
	int map_w = MAP_WIDTH / CHUNK_WIDTH;
	int map_h = MAP_HEIGHT / CHUNK_HEIGHT;
	int map_d = MAP_DEPTH / CHUNK_DEPTH;

	int size = map_w * map_h * map_d; // Array size

	chunks.resize(size);

	// Set all pointers to null
	for (unsigned int i = 0; i < size; ++i)
	{
		chunks[i] = nullptr;
	}

	std::cout << "Constructing Map object" << std::endl;
}

Map::~Map()
{
	// Find out how many chunks will fit in the map
	int map_w = MAP_WIDTH / CHUNK_WIDTH;
	int map_h = MAP_HEIGHT / CHUNK_HEIGHT;
	int map_d = MAP_DEPTH / CHUNK_DEPTH;

	int size = map_w * map_h * map_d; // Array size

	// Delete chunks
	for (unsigned int i = 0; i < size; ++i)
	{
		delete chunks[i];
		chunks[i] = nullptr;
	}
}

/// <summary>
/// Checks the map to see if it's empty.
/// </summary>
/// <returns>True if the map is empty.</returns>
bool Map::checkIsEmpty()
{
	// Find out how many chunks are in the map
	int map_w = MAP_WIDTH / CHUNK_WIDTH;
	int map_h = MAP_HEIGHT / CHUNK_HEIGHT;
	int map_d = MAP_DEPTH / CHUNK_DEPTH;

	for (int z = 0; z < map_d; ++z)
	{
		for (int y = 0; y < map_h; ++y)
		{
			for (int x = 0; x < map_w; ++x)
			{
				if (chunks[Utility::at(x, y, z, map_h, map_d)] != nullptr)
				{
					if (!chunks[Utility::at(x, y, z, map_h, map_d)]->checkIsEmpty())
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

/// <summary>
/// Sets a voxel to the specified type.
/// 0 = AIR / 1 = GRASS / 2 = WATER / 3 - TREE / 4 - LEAF
/// </summary>
/// <param name="x">The voxel's world position X value.</param>
/// <param name="y">The voxel's world position Y value.</param>
/// <param name="z">The voxel's world position Z value.</param>
/// <param name="type">The voxel type.</param>
void Map::setVoxel(int x, int y, int z, char type)
{
	// Check if voxel is a valid position within the bounds of the world map
	if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0 || z >= MAP_DEPTH || z < 0)
	{
		return;
	}

	// Get chunk array index
	int chunkX = std::floor(x / CHUNK_WIDTH);
	int chunkY = std::floor(y / CHUNK_HEIGHT);
	int chunkZ = std::floor(z / CHUNK_DEPTH);

	// Get voxel array index
	int voxX = std::floor(x % CHUNK_WIDTH);
	int voxY = std::floor(y % CHUNK_HEIGHT);
	int voxZ = std::floor(z % CHUNK_DEPTH);

	// Get chunk map array dimensions
	int map_h = MAP_HEIGHT / CHUNK_HEIGHT;
	int map_d = MAP_DEPTH / CHUNK_DEPTH;

	// Convert 3d index to 1d index
	int index = Utility::at(chunkX, chunkY, chunkZ, map_h, map_d);

	// If the voxel is being placed somewhere where a chunk doesn't
	// exist then create the chunk
	if (chunks[index] == nullptr)
	{
		chunks[index] = new Chunk();
	}

	// Set voxel type
	int voxelIndex = Utility::at(voxX, voxY, voxZ, CHUNK_HEIGHT, CHUNK_WIDTH);
	chunks[index]->voxels[voxelIndex] = type;

	// If the chunk is now empty then delete it
	if (chunks[index]->checkIsEmpty())
	{
		delete chunks[index];
		chunks[index] = nullptr;
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
char Map::getVoxel(int x, int y, int z)
{
	// Check if voxel is a valid position within the bounds of the world map
	if (x >= MAP_WIDTH || x < 0 || y >= MAP_HEIGHT || y < 0 || z >= MAP_DEPTH || z < 0)
	{
		return 0;
	}

	// Get chunk array index
	int chunkX = std::floor(x / CHUNK_WIDTH);
	int chunkY = std::floor(y / CHUNK_HEIGHT);
	int chunkZ = std::floor(z / CHUNK_DEPTH);

	// Get voxel array index
	int voxX = std::floor(x % CHUNK_WIDTH);
	int voxY = std::floor(y % CHUNK_HEIGHT);
	int voxZ = std::floor(z % CHUNK_DEPTH);

	// Get chunk map array dimensions
	int map_h = MAP_HEIGHT / CHUNK_HEIGHT;
	int map_d = MAP_DEPTH / CHUNK_DEPTH;

	// Convert 3d index to 1d index
	int index = Utility::at(chunkX, chunkY, chunkZ, map_h, map_d);
	int voxelIndex = Utility::at(voxX, voxY, voxZ, CHUNK_HEIGHT, CHUNK_WIDTH);

	// Return voxel type
	return chunks[index]->voxels[voxelIndex];
}

/// <summary>
/// This function populates the map with grass, water and trees.
/// 0 = AIR / 1 = GRASS / 2 = WATER / 3 - TREE / 4 - LEAF
/// </summary>
/// <param name="heightMap">An array of height map values.</param>
void Map::populate(int heightMap[MAP_WIDTH][MAP_DEPTH], int treeMap[MAP_WIDTH][MAP_DEPTH], int waterMap[MAP_WIDTH][MAP_DEPTH])
{
	for (int y = 0; y < MAP_DEPTH; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			setVoxel(x, heightMap[x][y] + 48, y, 1); // Grass
			setVoxel(x, waterMap[x][y] + 48, y, 2); // Water
		}
	}

	// placeScenery(treeMap, worldSize); // Place trees
	checkAllChunks(); // Check for empty chunks
}

/// <summary>
/// Populates the map with trees
/// </summary>
/// <param name="heightMap">An array of height map values.</param>
void Map::placeScenery(int treeMap[MAP_WIDTH][MAP_DEPTH], Indices worldSize)
{
	for (int y = 0; y < MAP_DEPTH; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
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
						// Because voxels can only be placed within the boundaries of the current map,
						// tree tops can sometimes end up generating over the edge of
						// the current map and into another. This will result in the tree being sliced vertically
						// where the map's edge is. To overcome this, a function pointer is binded to the setVoxel()
						// function from the World class. This function allows voxels to be placed anywhere in the
						// world and on any map.
						setVoxel(width * worldSize.x, height * worldSize.y + 48, depth * worldSize.z, 4); // Leaf
						//std::cout << width * worldSize.x << ", " << height * worldSize.y + 48 << ", " << depth * worldSize.z << std::endl;
					}
				}

				modifier += 1;
			}
		}
	}
}

void Map::checkAllChunks()
{
	// Find out how many chunks will fit in the map
	int map_w = MAP_WIDTH / CHUNK_WIDTH;
	int map_h = MAP_HEIGHT / CHUNK_HEIGHT;
	int map_d = MAP_DEPTH / CHUNK_DEPTH;

	for (int z = 0; z < map_d; ++z)
	{
		for (int y = 0; y < map_h; ++y)
		{
			for (int x = 0; x < map_w; ++x)
			{
				// If there is a null pointer at [x, y, z] then that chunk
				// is empty/doesn't exist and doesn't need to be checked
				if (chunks[Utility::at(x, y, z, map_h, map_d)] == nullptr)
				{
					continue;
				}
				// If the pointer is not null then check the chunk it's pointing to
				else if (chunks[Utility::at(x, y, z, map_h, map_d)]->checkIsEmpty())
				{
					delete chunks[Utility::at(x, y, z, map_h, map_d)]; // Delete the empty chunk
					chunks[Utility::at(x, y, z, map_h, map_d)] = nullptr; // Make pointer null
				}
			}
		}
	}
}
