#include "OpenGL.h"

/// <summary>
/// Import a model.
/// Only OBJ is currently supported.
/// If the t_model.instancingPositions array is empty, then the instance array buffer won't be used.
/// You can ignore the last parameter if there's no texture needed.
/// </summary>
/// <param name="t_modelFilename">The filename (including path) of the OBJ file.</param>
/// <param name="t_model">The data struct that stores the IDs and arrays for the model.</param>
/// <param name="t_diffuseTextureFilename">[OPTIONAL] The filename (including path) of the model's texture.</param>
void ab::OpenGL::import(const char *t_modelFilename, ab::Model &t_model, std::string t_diffuseTextureFilename)
{
	// If a filename is provided, then attempt to load a texture
	if (t_diffuseTextureFilename != "")
	{
		// This is used to hold the texture data
		unsigned char *f_data;

		// Width, height, texture component and colour type
		int f_width;
		int f_height;
		int f_compCount;
		const int f_number = 4;

		// Load diffuse texture (RGBA)
		stbi_set_flip_vertically_on_load(false);
		f_data = stbi_load(t_diffuseTextureFilename.c_str(), &f_width, &f_height, &f_compCount, 0);

		glGenTextures(1, &t_model.diffuseTextureID);
		glBindTexture(GL_TEXTURE_2D, t_model.diffuseTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f_width, f_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, f_data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(f_data); // Unload data from CPU as it's on the GPU now
	}
	else
	{
		std::cout << "No texture was provided so no attempt was made to load one" << std::endl;
	}
	
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

	// UV buffer
	glGenBuffers(1, &t_model.uvBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, t_model.uvBufferID);
	glBufferData(GL_ARRAY_BUFFER, t_model.uvs.size() * sizeof(glm::vec2), &t_model.uvs[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Normal buffer
	glGenBuffers(1, &t_model.normalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, t_model.normalBufferID);
	glBufferData(GL_ARRAY_BUFFER, t_model.normals.size() * sizeof(glm::vec3), &t_model.normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// EBO (used for indices)
	glGenBuffers(1, &t_model.elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t_model.elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, t_model.indices.size() * sizeof(unsigned short), &t_model.indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_UNSIGNED_SHORT, GL_FALSE, 0, (void*)0);

	if (t_model.instancingPositions.size() > 0)
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
/// Draw model(s).
/// You can ignore the last parameter if the model has no texture.
/// </summary>
/// <param name="t_model">The data struct that holds all model data.</param>
/// <param name="t_shader">Shader class object.</param>
/// <param name="t_uniformName>[OPTIONAL] The name of the uniform as it is in the shader. The model's texture gets sent to this uniform.</param>
void ab::OpenGL::draw(Model &t_model, Shader *t_shader, std::string t_uniformName)
{
	if (t_shader != nullptr)
	{
		

		if (t_uniformName != "")
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, t_model.diffuseTextureID);
			glUniform1i(glGetUniformLocation(t_shader->m_programID, t_uniformName.c_str()), 1);
		}
	}

	glBindVertexArray(t_model.vertexArrayObjectID);

	if (t_model.instancingPositions.size() == 0)
	{		
		glDrawElements(GL_TRIANGLES, t_model.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, t_model.instanceBufferID);
		glDrawElementsInstanced(GL_TRIANGLES, t_model.indices.size(), GL_UNSIGNED_SHORT, (void*)0, t_model.instancingPositions.size());
	}

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(0);
}

/// <summary>
/// Uniform 4fv
/// </summary>
/// <param name="t_shader">Shader class object.</param>
/// <param name="t_uniformName">The name of the uniform as it is in the shader.</param>
/// <param name="t_value">Points to the matrix that is to be passed to the specified shader uniform.</param>
void ab::OpenGL::uniformMatrix4fv(Shader &t_shader, std::string t_uniformName, GLfloat *t_value)
{
	glUniformMatrix4fv(glGetUniformLocation(t_shader.m_programID, t_uniformName.c_str()), 1, GL_FALSE, t_value);
}
