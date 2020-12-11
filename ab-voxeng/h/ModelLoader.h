#pragma warning(disable : 4996)

#ifndef MODELLOADER_H
#define MODELLOADER_H

#include "glew/glew.h"
#include "glm/glm.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

namespace ab
{
	class ModelLoader
	{
	private:
		struct PackedVertex
		{
			glm::vec3 position;
			glm::vec2 uv;
			glm::vec3 normal;

			bool operator<(const PackedVertex that) const
			{
				return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
			};
		};

		static bool processVertices(PackedVertex &t_packedVert, std::map<PackedVertex, unsigned short> &t_vertexToOutIndex, unsigned short &t_result);
		static void indexer(std::vector<glm::vec3> &t_inVertices, std::vector<glm::vec2> &t_inUvs, std::vector<glm::vec3> &t_inNormals,
			std::vector<unsigned short> &t_outIndices, std::vector<glm::vec3> &t_outVertices, std::vector<glm::vec2> &t_outUvs,
			std::vector<glm::vec3> &t_outNormals);

	public:
		static bool loadOBJ(const char *t_path, std::vector<glm::vec3> &t_outVertices, std::vector<glm::vec2> &t_outUvs,
			std::vector<glm::vec3> &t_outNormals, std::vector<unsigned short> &t_indices);
	};
}

#endif // !MODELLOADER_H
