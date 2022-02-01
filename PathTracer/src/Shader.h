#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

//#include <windows.h> 

#include <unordered_map>
#include <glm/glm.hpp>

#include <glad/glad.h>

struct ShaderSource
{
	const std::string vertex;
	const std::string fragment;
};

class Shader {
private:
	unsigned int m_RendererID;
	std::string m_FilePathVertex;
	std::string m_FilePathFragment;
	ShaderSource m_SourceCode;
	mutable std::unordered_map<std::string, int> m_UniformLocationCash; 

public:
	Shader(const std::string& filepathVertex, const std::string& filepathFragment);
	~Shader();

	void Bind() const;
	void Unbind() const;

	//Set uniforms
	void SetUniform1i(const std::string& name, int v0);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform1f(const std::string &name, float v0);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);


private:
	int GetUniformLocation(const std::string &name) const;
	unsigned int CreateShader();
	ShaderSource ParseShader();
	unsigned int CreateSubShader(unsigned int type, const char* source);
	bool CheckCompileErrors(unsigned int shader, unsigned int type);
};