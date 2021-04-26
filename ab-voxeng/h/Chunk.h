// *******************************
// * Chunk.h - Alan Bolger, 2020 *
// *******************************
// Chunks store voxels (For now, hehe). A voxel is represented by a byte (char) because it is smol.
//
// 0 = AIR
// 1 = GRASS
// 2 = WATER
// 3 - TREE
// 4 - LEAF
//
// Yeah, so that's it. Bye.

#ifndef CHUNK_H
#define CHUNK_H

#include "Voxel.h"

class Chunk
{
public:
	Chunk()
	{
		int x = 16;
		int y = 16;
		int z = 16;
		int size = x * y * z;

		voxels.resize(size);

		for (unsigned int i = 0; i < size; ++i)
		{
			voxels[i] = 0; // 0 is air
		}
	};

	~Chunk()
	{
		//int x = 16;
		//int y = 16;
		//int z = 16;
		//int size = x * y * z;

		//for (unsigned int i = 0; i < size; ++i)
		//{
		//	delete voxels[i];
		//}
	};

	/// <summary>
	/// Checks the chunk to see if it's empty.
	/// </summary>
	/// <returns>True if the chunk is empty.</returns>
	bool checkIsEmpty()
	{
		for (int z = 0; z < 16; ++z)
		{
			for (int y = 0; y < 16; ++y)
			{
				for (int x = 0; x < 16; ++x)
				{
					if (voxels[at(x, y, z)] != 0)
					{
						return false;						
					}
				}
			}
		}

		return true;
	};

	/// <summary>
	/// Use this to convert a 3D array index value into a 1D array index value.
	/// </summary>
	/// <param name="x">The X value.</param>
	/// <param name="y">The Y value.</param>
	/// <param name="z">The Z value.</param>
	/// <returns>A 1D array index value.</returns>
	int at(int x, int y, int z)
	{
		int map_h = 16;
		int map_d = 16;

		return x * map_h * map_d + y * map_d + z;
	}

	/// <summary>
	/// Use this to convert a 1D array index value into a 3D array index value.
	/// </summary>
	/// <param name="i">The 1D index value.</param>
	/// <returns>A 3D array index value.</returns>
	Indices at(int i)
	{
		int map_h = 16;
		int map_d = 16;

		Indices indices;
		indices.x = i / (map_h * map_d);
		indices.y = (i - indices.x * map_h * map_d) / map_d;
		indices.z = i - indices.x * map_h * map_d - indices.y * map_d;

		return indices;
	}

	std::vector<char> voxels;
};

#endif // !CHUNK_H

