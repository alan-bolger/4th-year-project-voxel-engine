#include "Terrain.h"
#include <iostream>

/// <summary>
/// Constructor for the Terrain class.
/// </summary>
/// <param name="t_width">The width of the window.</param>
/// <param name="t_height">The height of the window.</param>
ab::Terrain::Terrain()
{
	initialise();
}

/// <summary>
/// Constructor for the Terrain class.
/// </summary>
/// <param name="t_width">The width of the window.</param>
/// <param name="t_height">The height of the window.</param>
ab::Terrain::~Terrain()
{
	delete m_noise;
	m_noise = nullptr;
}

/// <summary>
/// Initialise the Terrain class.
/// </summary>
void ab::Terrain::initialise()
{
	m_noise = new Noise();

	// This seed can be used to regenerate the exact same world every time
	std::srand(12345);
}

/// <summary>
/// Generate a terrain map.
/// </summary>
/// <param name="t_width">The width of the map.</param>
/// <param name="t_height">The height of the map.</param>
void ab::Terrain::generate(int t_width, int t_height)
{
	// Temporary storage for map values
	std::vector<std::vector<float>> f_elevationMap;
	std::vector<std::vector<float>> f_treeMap;

	// Initialise arrays
	f_elevationMap.resize(t_width, std::vector<float>(t_height));
	f_treeMap.resize(t_width, std::vector<float>(t_height));

	// Create random values using seed
	float val_1 = rand() % 9 + 1;

	// Generate terrain
	for (int y = 0; y < t_height; ++y)
	{
		for (int x = 0; x < t_width; ++x)
		{
			float freqX = x / (float)t_width - 0.5f;
			float freqY = y / (float)t_height - 0.5f;
			float distance = sqrt(freqX * freqX + freqY * freqY) / sqrt(0.5f);
			distance = pow(distance, 2.0);

			double e = (1.00f * m_noise->noise(1.0 * (double)freqX, 1.0 * (double)freqY)
				+ 0.50f * m_noise->noise(2.0 * (double)freqX, 2.0 * (double)freqY)
				+ 0.25f * m_noise->noise(4.0 * (double)freqX, 4.0 * (double)freqY)
				+ 0.13f * m_noise->noise(8.0 * (double)freqX, 8.0 * (double)freqY)
				+ 0.06f * m_noise->noise(16.0 * (double)freqX, 16.0 * (double)freqY)
				+ 0.03f * m_noise->noise(32.0 * (double)freqX, 32.0 * (double)freqY));

			e /= (3.00 + 0.50 + 0.25 + 0.13 + 0.06 + 0.03);
			//e = (1 + e - distance) / 2; // Uncommenting this line will produce maps like islands
			e = pow(e, EXP);
			f_elevationMap[x][y] = e;
		}
	}

	float f_min = 0;
	float f_max = 0;

	// Initialise min and max from first two elevation map elements
	if (f_elevationMap[0][0] > f_elevationMap[0][1])
	{
		f_max = f_elevationMap[0][0];
		f_min = f_elevationMap[0][1];
	}
	else
	{
		f_max = f_elevationMap[0][1];
		f_min = f_elevationMap[0][0];
	}

	// First pass of array to find MIN and MAX values
	for (int z = 0; z < t_height; ++z)
	{
		for (int x = 0; x < t_width; ++x)
		{
			float f_yValue = (f_elevationMap[x][z]);

			// Set min and max values from array
			if (f_yValue > f_max)
			{
				f_max = f_yValue;
			}
			else if (f_yValue < f_min)
			{
				f_min = f_yValue;
			}
		}
	}

	float f_yValue = 0.0f;
	float f_waterHeight = WATER_HEIGHT;

	// Second pass of array to normalise values using MIN and MAX
	for (int z = 0; z < t_height; ++z)
	{
		for (int x = 0; x < t_width; ++x)
		{
			float f_arrVal = f_elevationMap[x][z];
			f_yValue = m_noise->normaliseToRange(f_arrVal, f_min, f_max) * 63.0f;
			heightMap[x][z] = int(f_yValue);

			if (f_arrVal < f_waterHeight)
			{
				waterMap[x][z] = f_waterHeight - 1;
			}
			else
			{
				waterMap[x][z] = 0;
			}
			
			//treeMap[x][z] = int(f_yValue);
		}
	}

	// Higher frequency value means higher tree density
	int R = 8;
	float freq = 16.0f;

	for (int y = 0; y < t_height; y++)
	{
		for (int x = 0; x < t_width; x++)
		{
			double nx = x / (double)t_width - 0.5;
			double ny = y / (double)t_height - 0.5;
			f_treeMap[x][y] = m_noise->noise(freq * nx, freq * ny);
		}
	}

	// Set max value
	for (int yc = 0; yc < t_height; yc++)
	{
		for (int xc = 0; xc < t_width; xc++)
		{
			double max = 0;

			for (int yn = yc - R; yn <= yc + R; yn++)
			{
				for (int xn = xc - R; xn <= xc + R; xn++)
				{
					if (0 <= yn && yn < t_height && 0 <= xn && xn < t_width)
					{
						float e = f_treeMap[xn][yn];

						if (e > max)
						{
							max = e;
						}
					}
				}
			}

			// Place a tree is the current value is equal to the max value
			if (f_treeMap[xc][yc] == max)
			{
				if (heightMap[xc][yc] + 1 > f_waterHeight)
				{
					treeMap[xc][yc] = heightMap[xc][yc] + 1;
				}
			}
			else
			{
				treeMap[xc][yc] = 0;
			}
		}
	}
}
