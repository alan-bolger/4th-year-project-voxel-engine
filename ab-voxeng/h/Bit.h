// *****************************
// * Bit.h - Alan Bolger, 2020 *
// *****************************

#ifndef BIT_H
#define BIT_H

#include "Voxel.h"

class Bit
{
public:
	Bit()
	{
		int X = 4;
		int Y = 4;
		int Z = 4;

		voxels = new Voxel **[64];

		for (int i = 0; i < X; i++)
		{
			voxels[i] = new Voxel *[Y];

			for (int j = 0; j < Y; j++)
			{
				voxels[i][j] = new Voxel[Z];
			}
		}
	};

	~Bit()
	{
		int X = 4;
		int Y = 4;
		int Z = 4;

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

	Voxel ***voxels;
	bool empty = true;
};

#endif // !BIT_H

