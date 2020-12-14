#include "Terrain.h"

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
}

/// <summary>
/// Initialise the Terrain class.
/// </summary>
void ab::Terrain::initialise()
{
	m_noise = new Noise();
}

/// <summary>
/// Generate a terrain map.
/// </summary>
/// <param name="t_width">The width of the map.</param>
/// <param name="t_height">The height of the map.</param>
void ab::Terrain::generate(int t_width, int t_height)
{
	// Make sure the map is empty
	// clear(m_boxes);

	// Temporary storage for map values
	std::vector<std::vector<float>> f_elevationMap;
	std::vector<std::vector<float>> f_treeMap;

	// Initialise arrays
	f_elevationMap.resize(t_width, std::vector<float>(t_height));
	f_treeMap.resize(t_width, std::vector<float>(t_height));

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

			e /= (1.00 + 0.50 + 0.25 + 0.13 + 0.06 + 0.03);
			e = (1 + e - distance) / 2;
			e = pow(e, 4.00);
			f_elevationMap[y][x] = e;
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
	for (int z = 0; z < 4; ++z)
	{
		for (int x = 0; x < 4; ++x)
		{
			float f_yValue = (f_elevationMap[z][x]);

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
	float f_waterHeight = 18.0f;

	glm::vec3 f_boundingBoxSize = glm::vec3(1, 1, 1);

	// Second pass of array to normalise values using MIN and MAX
	for (int z = 0; z < 4; ++z)
	{
		for (int x = 0; x < 4; ++x)
		{
			float f_arrVal = f_elevationMap[z][x];
			f_yValue = m_noise->normaliseToRange(f_arrVal, f_min, f_max) * 63.0f;

			m_boxes[z][x].first = glm::vec3(x, f_yValue, z) - f_boundingBoxSize;
			m_boxes[z][x].second = glm::vec3(x, f_yValue, z) + f_boundingBoxSize;
		}
	}
}
