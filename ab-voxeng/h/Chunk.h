// *******************************
// * Chunk.h - Alan Bolger, 2020 *
// *******************************

#ifndef CHUNK_H
#define CHUNK_H

#include "Bit.h"

class Chunk
{
public:
	Chunk()
	{
		int X = 4;
		int Y = 4;
		int Z = 4;

		bits = new Bit **[64];

		for (int i = 0; i < X; i++)
		{
			bits[i] = new Bit * [Y];

			for (int j = 0; j < Y; j++)
			{
				bits[i][j] = new Bit[Z];
			}
		}
	};

	~Chunk()
	{
		int X = 4;
		int Y = 4;
		int Z = 4;

		for (int i = 0; i < X; i++)
		{
			for (int j = 0; j < Y; j++)
			{
				delete[] bits[i][j];
			}

			delete[] bits[i];
		}

		delete[] bits;
	};

	Bit ***bits;
	bool empty = true;
};

#endif // !CHUNK_H

