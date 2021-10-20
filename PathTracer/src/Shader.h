#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
//#include <windows.h> 

#include <unordered_map>

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
	std::unordered_map<std::string, int> m_UniformLocationCash; 

public:
	Shader(const std::string& filepathVertex, const std::string& filepathFragment);
	~Shader();

	void bind() const;
	void unbind() const;

	//Set uniforms
	void setUniform1i(const std::string& name, int v0);
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniform1f(const std::string &name, float v0);
	
private:
	int getUniformLocation(const std::string &name);
	unsigned int createShader();
	ShaderSource parseShader();
	unsigned int createSubShader(unsigned int type, const char* source);
	bool checkCompileErrors(unsigned int shader, unsigned int type);
};