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
	unsigned int m_IrradianceMap; //Карта облученности, свернутая кубмапа
	unsigned int m_HDRITextureID; //Hdri texture

	unsigned int m_CaptureFBO, m_CaptureRBO;
	std::string m_Path;
	int m_Width, m_Height, m_BPP;

	int m_OutputSize;

    VertexArray m_VA;

	Shader m_GenCubemapShader;
	Shader m_BackgroundShader;
	Shader m_IrradianceShader;

	void CreateCubemap(unsigned int& textureId, unsigned int size);
	void ConvertHDRIToCubemap();
	void BakeIrradianceMap();
	void RenderToCubemap(Shader& shader, GLint sourceType, unsigned int& from, unsigned int& to, int& size);
	void RenderCube(const VertexArray& va, const Shader& shader);

public:

	EnvironmentMap(const char* path, int outputSize = 512);

	void LoadEnvironmentMap(const char* path, int outputSize = 512);

	void Draw(glm::mat4 view, glm::mat4 projection);

	void BindIrradianceMap(unsigned int slot);
	inline const unsigned int& GetEnvironmentMap() { return m_RendererID; }
	inline const unsigned int& GetIrradianceMap() { return m_IrradianceMap; }
	inline std::string& GetPath() { return m_Path; }
	inline glm::vec2 GetSize() { return glm::vec2(m_Width, m_Height); }
};