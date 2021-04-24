// *******************************
// * Chunk.h - Alan Bolger, 2020 *
// *******************************

#ifndef CHUNK_H
#define CHUNK_H

#include "Voxel.h"

class Chunk
{
public:
	Chunk()
	{
		int X = 16;
		int Y = 16;
		int Z = 16;

		voxels = new Voxel **[4096];

		for (int i = 0; i < X; i++)
		{
			voxels[i] = new Voxel * [Y];

			for (int j = 0; j < Y; j++)
			{
				voxels[i][j] = new Voxel[Z];
			}
		}

		for (int z = 0; z < 16; ++z)
		{
			for (int y = 0; y < 16; ++y)
			{
				for (int x = 0; x < 16; ++x)
				{
					voxels[x][y][z] = Voxel::AIR;
				}
			}
		}
	};

	~Chunk()
	{
		int X = 16;
		int Y = 16;
		int Z = 16;

		for (int i = 0; i < X; i++)
		{
			for (int j = 0; j < Y; j++)
			{
				delete[] voxels[i][j];
			}

			delete[] voxels[i];
		}

		delete[] voxels;
	};

	/// <summary>
	/// Checks the chunk to see if it's empty.
	/// If it's empty then the chunk flag is set to true.
	/// </summary>
	void check()
	{
		empty = true;

		for (int z = 0; z < 16; ++z)
		{
			for (int y = 0; y < 16; ++y)
			{
				for (int x = 0; x < 16; ++x)
				{
					if (voxels[x][y][z] != Voxel::AIR)
					{
						empty = false;
						return;
					}
				}
			}
		}
	};

	Voxel ***voxels;
	bool empty = true;
};

#endif // !CHUNK_H

