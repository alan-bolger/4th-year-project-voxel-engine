#include "OpenGL.h"

/// <summary>
/// Import a model.
/// </summary>
/// <param name="t_modelFilename">The filename (including path) of the OBJ file.</param>
/// <param name="t_model">The data struct that stores the IDs and arrays for the model.</param>
/// <param name="t_instancingOn">Set this to true to use instancing. Instancing positions are stored in t_model.instancingPositions</param>
void ab::OpenGL::importModel(const char *t_modelFilename, Model &t_model, bool t_instancingOn)
{
	// This is used to hold the texture data
	// unsigned char *f_data;
	
	// Width, height, texture component and colour type
	// int f_width;
	// int f_height;
	// int f_compCount;
	// const int f_number = 4;
	
	// Load diffuse texture (RGBA)
	// stbi_set_flip_vertically_on_load(false);
	// f_data = stbi_load(t_texture.c_str(), &f_width, &f_height, &f_compCount, 0);
	
	// glGenTextures(1, &t_model.diffuseTextureID);
	// glBindTexture(GL_TEXTURE_2D, t_model.diffuseTextureID);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f_width, f_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, f_data);
	
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	// stbi_image_free(f_data); // Unload data from CPU as it's on the GPU now
	
	// Load OBJ file
	if (!ab::ModelLoader::loadOBJ(t_modelFilename, t_model.vertices, t_model.uvs, t_model.normals, t_model.indices))
	{
		std::cout << "Error loading model!" << std::endl;
	}

	// This VAO stores all draw states below
	glGenVertexArrays(1, &t_model.vertexArrayObjectID);
	glBindVertexArray(t_model.vertexArrayObjectID);

	// Vertex buffer
	glGenBuffers(1, &t_model.vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, t_model.vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, t_model.vertices.size() * sizeof(glm::vec3), &t_model.vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Normal buffer
	glGenBuffers(1, &t_model.normalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, t_model.normalBufferID);
	glBufferData(GL_ARRAY_BUFFER, t_model.normals.size() * sizeof(glm::vec3), &t_model.normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// UV buffer
	// glGenBuffers(1, &t_model.uvBufferID);
	// glBindBuffer(GL_ARRAY_BUFFER, t_model.uvBufferID);
	// glBufferData(GL_ARRAY_BUFFER, t_model.uvs.size() * sizeof(glm::vec2), &t_model.uvs[0], GL_STATIC_DRAW);

	// glEnableVertexAttribArray(2);
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// EBO (used for indices)
	glGenBuffers(1, &t_model.elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t_model.elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, t_model.indices.size() * sizeof(unsigned short), &t_model.indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_UNSIGNED_SHORT, GL_FALSE, 0, (void*)0);

	if (t_instancingOn && t_model.instancingPositions.size() > 0)
	{
		// Instance array buffer
		glGenBuffers(1, &t_model.instanceBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, t_model.instanceBufferID);
		glBufferData(GL_ARRAY_BUFFER, t_model.instancingPositions.size() * sizeof(glm::mat4), &t_model.instancingPositions[0], GL_STATIC_DRAW);

		std::size_t vec4Size = sizeof(glm::vec4);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glVertexAttribDivisor(6, 1);

		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glVertexAttribDivisor(7, 1);

		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glVertexAttribDivisor(8, 1);

		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
		glVertexAttribDivisor(9, 1);
	}

	glBindVertexArray(0);
}

/// <summary>
/// Draw a single model.
/// </summary>
/// <param name="t_model">The data struct that holds all model data.</param>
void ab::OpenGL::drawModel(Model &t_model)
{
	glBindVertexArray(t_model.vertexArrayObjectID);

	glDrawElements(GL_TRIANGLES, t_model.indices.size(), GL_UNSIGNED_SHORT, (void*)0);

	glBindVertexArray(0);
}

/// <summary>
/// Draw a model using instancing.
/// Instancing positions are stored in the t_model object as t_model.instancingPositions.
/// </summary>
/// <param name="t_model">The data struct that holds all model data.</param>
/// <param name="t_amount">This is the number of models you want to draw in one instance.</param>
void ab::OpenGL::drawModelsInstanced(Model &t_model)
{
	glBindVertexArray(t_model.vertexArrayObjectID);

	glBindBuffer(GL_ARRAY_BUFFER, t_model.instanceBufferID);
	glDrawElementsInstanced(GL_TRIANGLES, t_model.indices.size(), GL_UNSIGNED_SHORT, (void*)0, t_model.instancingPositions.size());

	glBindVertexArray(0);
}
