#include "Shader.h"

/// <summary>
/// Constructor for the Shader class.
/// </summary>
/// <param name="t_vertShader">Vertex shader file path.</param>
/// <param name="t_fragShader">Fragment shader file path.</param>
ab::Shader::Shader(const std::string &t_vertShader, const std::string &t_fragShader)
{
	std::string f_vertexShader = readFile(t_vertShader);
	std::string f_fragmentShader = readFile(t_fragShader);
	createShader(f_vertexShader, f_fragmentShader);
	glUseProgram(m_programID);
}

/// <summary>
/// Destructor for the Shader class.
/// </summary>
ab::Shader::~Shader()
{
	if (m_programID != 0)
	{
		glDeleteProgram(m_programID);
	}
}

/// <summary>
/// Opens and parses an external file.
/// </summary>
/// <param name="t_filePath">File path to open.</param>
std::string ab::Shader::readFile(const std::string &t_filePath)
{
	std::ifstream f_fileStream(t_filePath, std::ios::in);

	if (!f_fileStream.is_open())
	{
		std::cerr << "Could not read file " << t_filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::stringstream f_buffer;
	std::string f_line;

	while (std::getline(f_fileStream, f_line))
	{
		f_buffer << f_line << "\n";
	}

	f_fileStream.close();
	return f_buffer.str();
}

/// <summary>
/// Compile shader.
/// </summary>
/// <param name="t_type">Shader type.</param>
/// <param name="t_source">File path of shader.</param>
GLuint ab::Shader::compileShader(GLuint t_type, const std::string &t_source)
{
	GLuint f_shader = glCreateShader(t_type);

	// Load shader program into shader object
	const char *src = t_source.c_str();
	glShaderSource(f_shader, 1, &src, nullptr);

	// Compile the shader program
	glCompileShader(f_shader);

	GLint f_result;

	// Get result of shader compilation
	glGetShaderiv(f_shader, GL_COMPILE_STATUS, &f_result);

	if (f_result == GL_FALSE)
	{
		int f_length;

		// Assigns length with length of information log
		glGetShaderiv(f_shader, GL_INFO_LOG_LENGTH, &f_length);

		// Allocate on stack frame of caller
		char *infoLog = (char*)_malloca(f_length * sizeof(char));

		// Returns the information log for a shader object
		glGetShaderInfoLog(f_shader, f_length, &f_length, infoLog);

		// Display error message in console window
		std::cout << "Shader compiling failed: " << (t_type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << std::endl;
		std::cout << infoLog << std::endl;

		// Delete shader (it's of no use anymore!)
		glDeleteShader(f_shader);

		return static_cast<GLuint>(0);
	}

	return f_shader;
}

/// <summary>
/// Create shader.
/// </summary>
/// <param name="t_vertShader">Vertex shader file path.</param>
/// <param name="t_fragShader">Fragment shader file path.</param>
void ab::Shader::createShader(const std::string &t_vertShader, const std::string &t_fragShader)
{
	// Compile the two shaders given as string reference
	GLuint f_vs = compileShader(GL_VERTEX_SHADER, t_vertShader);
	GLuint f_fs = compileShader(GL_FRAGMENT_SHADER, t_fragShader);

	if (f_vs == 0 || f_fs == 0)
	{
		return;
	}

	// Create a container to attach shader objects to
	m_programID = glCreateProgram();

	// Attach the shader objects to the program object
	glAttachShader(m_programID, f_vs);
	glAttachShader(m_programID, f_fs);

	// Links all the shader objects together
	glLinkProgram(m_programID);

	GLint f_result;

	// Checks if everything has been successfully linked
	glGetShaderiv(m_programID, GL_COMPILE_STATUS, &f_result);

	if (f_result == GL_FALSE)
	{
		int f_length;

		// Assigns length with length of information log
		glGetShaderiv(m_programID, GL_INFO_LOG_LENGTH, &f_length);

		// Allocate on stack frame of caller (was originally using 'alloca', changed to '_malloca')
		char *f_infoLog = (char*)_malloca(f_length * sizeof(char));

		// Returns the information log for a shader object
		glGetShaderInfoLog(m_programID, f_length, &f_length, f_infoLog);

		// Display error message in console window
		std::cout << "Vertex and fragment shader link failed." << std::endl;
		std::cout << f_infoLog << std::endl;

		// Delete shader
		glDeleteProgram(m_programID);
	
		return;
	}

	glValidateProgram(m_programID);

	// Deletes intermediate objects
	glDeleteShader(f_vs);
	glDeleteShader(f_fs);
}