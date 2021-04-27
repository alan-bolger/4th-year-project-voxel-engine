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

class Ray
{
public:
	// TODO: Consider putting this into a struct possibly
	Ray(const glm::vec3& orig, const glm::vec3& dir) : orig(orig), dir(dir)
	{
		invdir = 1.0f / dir;
		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);
	}

	glm::vec3 orig, dir; // ray orig and dir 
	glm::vec3 invdir;
	int sign[3];
};

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

	/// <summary>
	/// This function checks every cube in the world to see if a ray has intersected with it.
	/// Currently, this function is horrible and inefficient but will do for testing purposes.
	/// </summary>
	/// <param name="t_origin">The ray's origin point.</param>
	/// <param name="t_direction">The ray's direction.</param>
	/// <param name="t_hitInfo">Stores the information from the cube that was intersected.</param>
	/// <returns>True if an intersection has occurred.</returns>
	bool checkAllCubesIntersect(glm::vec3 t_origin, glm::vec3 t_direction, Indices chunkIndex, HitInfo &t_hitInfo)
	{
		float f_smallest = voxels.size();
		bool f_found = false;

		for (int z = 0; z < 16; ++z)
		{
			for (int y = 0; y < 16; ++y)
			{
				for (int x = 0; x < 16; ++x)
				{
					if (voxels[at(x, y, z)] != 1)
					{
						continue;
					}

					glm::vec2 f_lambda = intersectCube(t_origin, t_direction, chunkIndex, glm::vec3(chunkIndex.x * 16 + x, chunkIndex.y * 16 + y, chunkIndex.z * 16 + z));

					if (f_lambda.x > 0.0 && f_lambda.x < f_lambda.y && f_lambda.x < f_smallest)
					{
						t_hitInfo.lambda = f_lambda;
						t_hitInfo.center = glm::vec3(chunkIndex.x * 16 + x, chunkIndex.y * 16 + y, chunkIndex.z * 16 + z);
						// t_hitInfo.bi = i;
						f_smallest = f_lambda.x;
						// m_selectedCube = m_voxelPositions[i];
						voxels[at(x, y, z)] = 0;
						f_found = true;
					}
				}
			}
		}

		return f_found;
	}

	/// <summary>
	/// Function to check if a cube is being intersected.
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

	bool checkChunkIntersect(int chunkX, int chunkY, int chunkZ, glm::vec3 t_origin, glm::vec3 t_direction, glm::vec3& t_hitPoint)
	{
		//Ray ray(t_origin, t_direction);
		//float t;

		//for (int z = 0; z < 16; ++z)
		//{
		//	for (int y = 0; y < 16; ++y)
		//	{
		//		for (int x = 0; x < 16; ++x)
		//		{
		//			if (intersect(ray, t, chunkX * 16 + x, chunkY * 16 + y, chunkZ * 16 + z))
		//			{
		//				std::cout << +voxels[at(x, y, z)] << std::endl;
		//				std::cout << "Chunk: " << chunkX << ", " << chunkY << ", " << chunkZ << std::endl;
		//				std::cout << "Voxel: " << x << ", " <<  y << ", " << z << std::endl;
		//				std::cout << "Intersect: " << chunkX * 16 + x << ", " << chunkY * 16 + y << ", " << chunkZ * 16 + z << std::endl;
		//				t_hitPoint = ray.orig + ray.dir * t; // This will be needed to add a voxel to the world using the mouse

		//				return true;
		//			}
		//		}
		//	}
		//}



		return false;
	}



	bool intersect(const Ray& r, float& t, int x, int y, int z) const
	{
		glm::vec3 bounds[2];
		glm::vec3 f_cubeSize(0.5, 0.5, 0.5);
		glm::vec3 f_cubeCenter = glm::vec3(x, y, z);

		// std::cout << x << ", " << y << ", " << z << std::endl;

		bounds[0] = f_cubeCenter - f_cubeSize;
		bounds[1] = f_cubeCenter + f_cubeSize;

		float tmin, tmax, tymin, tymax, tzmin, tzmax;

		tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
		tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
		tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
		tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;

		tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
		tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;

		t = tmin;

		if (t < 0) {
			t = tmax;
			if (t < 0) return false;
		}

		return true;
	}

	std::vector<char> voxels;
};

#endif // !CHUNK_H

