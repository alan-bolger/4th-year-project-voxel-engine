#include "ModelLoader.h"

/// <summary>
/// Find similar vertices.
/// </summary>
bool ab::ModelLoader::processVertices(PackedVertex &t_packedVert, std::map<PackedVertex, unsigned short> &t_vertexToOutIndex, unsigned short &t_result)
{
	std::map<PackedVertex, unsigned short>::iterator f_iterator = t_vertexToOutIndex.find(t_packedVert);

	if (f_iterator == t_vertexToOutIndex.end())
	{
		return false;
	}
	else
	{
		t_result = f_iterator->second;
		return true;
	}
}

/// <summary>
/// Create indices.
/// </summary>
void ab::ModelLoader::indexer(std::vector<glm::vec3> &t_inVertices, std::vector<glm::vec2> &t_inUvs, std::vector<glm::vec3> &t_inNormals,
	std::vector<unsigned short> &t_outIndices, std::vector<glm::vec3> &t_outVertices, std::vector<glm::vec2> &t_outUvs,
	std::vector<glm::vec3> &t_outNormals)
{
	std::map<PackedVertex, unsigned short> f_vertexToOutIndex;

	// For each input vertex
	for (unsigned int i = 0; i < t_inVertices.size(); i++)
	{
		PackedVertex f_packedVert = { t_inVertices[i], t_inUvs[i], t_inNormals[i] };
		unsigned short f_index;
		bool f_found = processVertices(f_packedVert, f_vertexToOutIndex, f_index);

		if (f_found)
		{
			t_outIndices.push_back(f_index);
		}
		else
		{
			t_outVertices.push_back(t_inVertices[i]);
			t_outUvs.push_back(t_inUvs[i]);
			t_outNormals.push_back(t_inNormals[i]);
			unsigned short f_newIndex = (unsigned short)t_outVertices.size() - 1;
			t_outIndices.push_back(f_newIndex);
			f_vertexToOutIndex[f_packedVert] = f_newIndex;
		}
	}
}

/// <summary>
/// Load an .obj model file.
/// </summary>
bool ab::ModelLoader::loadOBJ(const char *t_path, std::vector<glm::vec3> &t_outVertices, std::vector<glm::vec2> &t_outUvs,
	std::vector<glm::vec3> &t_outNormals, std::vector<unsigned short> &t_indices)
{
	std::vector<unsigned int> f_vertexIndices;
	std::vector<unsigned int> f_uvIndices;
	std::vector<unsigned int> f_normalIndices;

	std::vector<glm::vec3> f_tempVertices;
	std::vector<glm::vec2> f_tempUvs;
	std::vector<glm::vec3> f_tempNormals;

	std::vector<glm::vec3> f_tempOutVertices;
	std::vector<glm::vec2> f_tempOutUvs;
	std::vector<glm::vec3> f_tempOutNormals;

	FILE *f_file = std::fopen(t_path, "r");

	if (f_file == NULL)
	{
		std::cout << "File cannot be opened!" << std::endl;

		return false;
	}

	while (true)
	{
		char f_lineHeader[128];

		// Get the first word of the line
		int f_res = fscanf(f_file, "%s", f_lineHeader);

		if (f_res == EOF)
		{
			break; // End of file
		}

		if (strcmp(f_lineHeader, "v") == 0)
		{
			glm::vec3 f_vertex;
			fscanf(f_file, "%f %f %f\n", &f_vertex.x, &f_vertex.y, &f_vertex.z);
			f_tempVertices.push_back(f_vertex);
		}
		else if (strcmp(f_lineHeader, "vt") == 0)
		{
			glm::vec2 f_uv;
			fscanf(f_file, "%f %f\n", &f_uv.x, &f_uv.y);
			f_uv.y = -f_uv.y; // Invert V coordinate - remove this if you want to use TGA or BMP loaders
			f_tempUvs.push_back(f_uv);
		}
		else if (strcmp(f_lineHeader, "vn") == 0)
		{
			glm::vec3 f_normal;
			fscanf(f_file, "%f %f %f\n", &f_normal.x, &f_normal.y, &f_normal.z);
			f_tempNormals.push_back(f_normal);
		}
		else if (strcmp(f_lineHeader, "f") == 0)
		{
			std::string f_vertex1;
			std::string f_vertex2;
			std::string f_vertex3;
			unsigned int f_vertexIndex[3];
			unsigned int f_uvIndex[3];
			unsigned int f_normalIndex[3];
			int f_matches = fscanf(f_file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &f_vertexIndex[0], &f_uvIndex[0], &f_normalIndex[0],
				&f_vertexIndex[1], &f_uvIndex[1], &f_normalIndex[1], &f_vertexIndex[2], &f_uvIndex[2], &f_normalIndex[2]);

			if (f_matches != 9)
			{
				printf("File can't be read - make sure file is compatible!\n");
				fclose(f_file);
				return false;
			}

			f_vertexIndices.push_back(f_vertexIndex[0]);
			f_vertexIndices.push_back(f_vertexIndex[1]);
			f_vertexIndices.push_back(f_vertexIndex[2]);
			f_uvIndices.push_back(f_uvIndex[0]);
			f_uvIndices.push_back(f_uvIndex[1]);
			f_uvIndices.push_back(f_uvIndex[2]);
			f_normalIndices.push_back(f_normalIndex[0]);
			f_normalIndices.push_back(f_normalIndex[1]);
			f_normalIndices.push_back(f_normalIndex[2]);
		}
		else
		{
			// If it's a comment, destroy the rest of the line
			char f_tempBuffer[1000];
			fgets(f_tempBuffer, 1000, f_file);
		}
	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < f_vertexIndices.size(); i++)
	{
		// Get the indices of its attributes
		unsigned int f_vertexIndex = f_vertexIndices[i];
		unsigned int f_uvIndex = f_uvIndices[i];
		unsigned int f_normalIndex = f_normalIndices[i];

		// Get the attributes by using the index
		glm::vec3 f_vertex = f_tempVertices[f_vertexIndex - 1];
		glm::vec2 f_uv = f_tempUvs[f_uvIndex - 1];
		glm::vec3 f_normal = f_tempNormals[f_normalIndex - 1];

		// Put the attributes in buffers
		f_tempOutVertices.push_back(f_vertex);
		f_tempOutUvs.push_back(f_uv);
		f_tempOutNormals.push_back(f_normal);
	}

	fclose(f_file);

	ab::ModelLoader::indexer(f_tempOutVertices, f_tempOutUvs, f_tempOutNormals, t_indices, t_outVertices, t_outUvs, t_outNormals);

	return true;
}
