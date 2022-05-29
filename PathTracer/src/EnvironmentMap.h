#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

#include "vendor/stb_image/stb_image.h"
#include <string>
#include <glad/glad.h>
#include <iostream>

class EnvironmentMap
{
private:
	unsigned int m_RendererID; //Cubemap
	unsigned int m_HDRITextureID; //Hdri texture

	unsigned int m_CaptureFBO, m_CaptureRBO;
	std::string m_Path;
	int m_Width, m_Height, m_BPP;

	int m_OutputSize;

    VertexArray m_VA;

	Shader m_GenCubemapShader;
	Shader m_BackgroundShader;

	void BakeToFramebuffer();
	void RenderCube(const VertexArray& va, const Shader& shader);
	void BindTexture();

public:

	EnvironmentMap(const char* path, int outputSize = 512);

	void LoadEnvironmentMap(const char* path, int outputSize = 512);

	void Draw(glm::mat4 view, glm::mat4 projection);

	inline const unsigned int& GetTexture() { return m_RendererID; }
	inline VertexArray& GetVA() { return m_VA; }
	inline Shader& GetShader() { return m_GenCubemapShader; }
	inline std::string& GetPath() { return m_Path; }
	inline glm::vec2 GetSize() { return glm::vec2(m_Width, m_Height); }
};