// *******************************
// * Chunk.h - Alan Bolger, 2020 *
// *******************************

#ifndef CHUNK_H
#define CHUNK_H

#include <vector>

class Chunk
{
public:
	Chunk()
	{
		// Set chunk dimensions
		int x = CHUNK_WIDTH;
		int y = CHUNK_HEIGHT;
		int z = CHUNK_DEPTH;

		int size = x * y * z; // Array size

		voxels.resize(size);

		for (unsigned int i = 0; i < size; ++i)
		{
			voxels[i] = 0; // Initialise chunk with air
		}
	}

	~Chunk()
	{

	}

	/// <summary>
	/// Checks the chunk to see if it's empty.
	/// </summary>
	/// <returns>True if the chunk is empty.</returns>
	bool checkIsEmpty()
	{
		for (int z = 0; z < CHUNK_DEPTH; ++z)
		{
			for (int y = 0; y < CHUNK_HEIGHT; ++y)
			{
				for (int x = 0; x < CHUNK_WIDTH; ++x)
				{
					// If any of the blocks are not air then the chunk isn't empty
					if (voxels[Utility::at(x, y, z, CHUNK_HEIGHT, CHUNK_DEPTH)] != 0)
					{
						return false;						
					}
				}
			}
		}

		return true;
	};

	/// <summary>
	/// This function checks every voxel / cube in the chunk for a ray intersect.
	/// </summary>
	/// <param name="t_origin">The ray's origin point.</param>
	/// <param name="t_direction">The ray's direction.</param>
	/// <param name="t_hitInfo">Stores the information from the cube that was intersected.</param>
	/// <returns>True if an intersection has occurred.</returns>
	bool checkAllCubesIntersect(glm::vec3 t_origin, glm::vec3 t_direction, Indices chunkIndex, HitInfo &hitInfo)
	{
		float f_smallest = voxels.size();
		bool f_found = false;

		for (int z = 0; z < 16; ++z)
		{
			for (int y = 0; y < 16; ++y)
			{
				for (int x = 0; x < 16; ++x)
				{
					if (voxels[Utility::at(x, y, z, CHUNK_HEIGHT, CHUNK_WIDTH)] != 1)
					{
						continue;
					}

					glm::vec2 f_lambda = intersectCube(t_origin, t_direction, chunkIndex, glm::vec3(chunkIndex.x * 16 + x, chunkIndex.y * 16 + y, chunkIndex.z * 16 + z));

					if (f_lambda.x > 0.0 && f_lambda.x < f_lambda.y && f_lambda.x < f_smallest)
					{
						hitInfo.lambda = f_lambda;
						hitInfo.center = glm::vec3(chunkIndex.x * 16 + x, chunkIndex.y * 16 + y, chunkIndex.z * 16 + z);
						f_smallest = f_lambda.x;
						f_found = true;
					}
				}
			}
		}

		return f_found;
	}

	/// <summary>
	/// Function to check if a voxel / cube is being intersected.
	/// </summary>
	/// <param name="t_origin">The ray's origin point.</param>
	/// <param name="t_direction">The ray's direction.</param>
	/// <param name="t_cubeCenter">The center of the cube being checked.</param>
	/// <returns>The near and far values.</returns>
	glm::vec2 intersectCube(glm::vec3 t_origin, glm::vec3 t_direction, Indices chunks, glm::vec3 cubeCenter)
	{
		glm::vec3 f_cubeSize(0.5, 0.5, 0.5);

		glm::vec3 f_min = ((cubeCenter - f_cubeSize) - t_origin) / t_direction;
		glm::vec3 f_max = ((cubeCenter + f_cubeSize) - t_origin) / t_direction;
		glm::vec3 f_t1 = glm::min(f_min, f_max);
		glm::vec3 f_t2 = glm::max(f_min, f_max);

		float f_near = std::max(std::max(f_t1.x, f_t1.y), f_t1.z);
		float f_far = std::min(std::min(f_t2.x, f_t2.y), f_t2.z);

		return glm::vec2(f_near, f_far);
	}

	std::vector<char> voxels;
};

#endif // !CHUNK_H

