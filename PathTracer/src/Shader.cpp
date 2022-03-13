#include "Shader.h"

Shader::Shader(const std::string& filepathVertex, const std::string& filepathFragment)
	: m_FilePathVertex(filepathVertex)
	, m_FilePathFragment(filepathFragment)
	, m_FilePathGeometry("")
	, m_RendererID(0)
{
	m_RendererID = CreateShader();

	/*HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 116);*/

	if (m_RendererID == 0)
		std::cout << "ERROR::SHADER::SHADER_CREATION_FAILED" << std::endl;

	//SetConsoleTextAttribute(hConsole, 0);
}

Shader::Shader(const std::string& filepathVertex, const std::string& filepathFragment, const std::string& filepathGeometry )
	: m_FilePathVertex(filepathVertex)
	, m_FilePathFragment(filepathFragment)
	, m_FilePathGeometry(filepathGeometry)
	, m_RendererID(0)
{
	m_RendererID = CreateShader();

	/*HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 116);*/

	if (m_RendererID == 0)
		std::cout << "ERROR::SHADER::SHADER_CREATION_FAILED" << std::endl;

	//SetConsoleTextAttribute(hConsole, 0);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

unsigned int Shader::CreateShader()
{
	ShaderSource source = ParseShader();

	const char* vShaderCode = source.vertex.c_str();
	const char* fShaderCode = source.fragment.c_str();
	const char* gShaderCode = source.geometry.c_str();

	unsigned int vertexID, fragmentID, geometryID = -1;
	vertexID = CreateSubShader(GL_VERTEX_SHADER, vShaderCode);
	if (vertexID == 0)
		return 0;
	fragmentID = CreateSubShader(GL_FRAGMENT_SHADER, fShaderCode);
	if (fragmentID == 0)
		return 0;
	if (source.geometry != "") {
		geometryID = CreateSubShader(GL_GEOMETRY_SHADER, gShaderCode);
		if (geometryID == 0)
			return 0;
	}

	unsigned int shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexID);
	if(gShaderCode != "")
		glAttachShader(shaderProgramID, geometryID);
	glAttachShader(shaderProgramID, fragmentID);
	glLinkProgram(shaderProgramID);

	if (!CheckCompileErrors(shaderProgramID, 1))
		return 0;
	
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	if (gShaderCode != "") 
		glDeleteShader(geometryID);

	return shaderProgramID;
}

ShaderSource Shader::ParseShader() {
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	if(m_FilePathGeometry != "")
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(m_FilePathVertex);
		fShaderFile.open(m_FilePathFragment);
		if (m_FilePathGeometry != "")
			gShaderFile.open(m_FilePathGeometry);
		std::stringstream vShaderStream, fShaderStream, gShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		if (m_FilePathGeometry != "")
			gShaderStream << gShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();
		if (m_FilePathGeometry != "")
			gShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (m_FilePathGeometry != "")
			geometryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: \"" << m_FilePathVertex << "\", \"" << m_FilePathFragment << "\"" << std::endl;
	}

	if(m_FilePathGeometry != "")
		return { vertexCode, fragmentCode, geometryCode };
	else
		return { vertexCode, fragmentCode };
}

unsigned int Shader::CreateSubShader(unsigned int type, const char* source) {
	int shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, &source, NULL);
	glCompileShader(shaderID);

	if (!CheckCompileErrors(shaderID, type))
		return 0;

	return shaderID;
}

bool Shader::CheckCompileErrors(unsigned int shader, unsigned int type) {
	int success;
	char infoLog[512];

	if (type == GL_VERTEX_SHADER) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			return false;
		}
	}
	else if (type == GL_FRAGMENT_SHADER) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			return false;
		}
	}
	else if (type == GL_GEOMETRY_SHADER) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
			return false;
		}
	}
	else {
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			return false;
		}
	}
	return true;
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int v0)
{
	glUniform1i(GetUniformLocation(name), v0);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
	glUniform1f(GetUniformLocation(name), v0);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string& name) const
{
	if (m_UniformLocationCash.find(name) != m_UniformLocationCash.end())
		return m_UniformLocationCash[name];

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		std::cout << "WARNING:: uniform '" << name << "' doesn't exist!\n";
	
	m_UniformLocationCash[name] = location;
	return location;
}
