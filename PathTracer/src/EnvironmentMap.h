#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

#include "vendor/stb_image/stb_image.h"
#include <string>
#include <glad/glad.h>
#include <iostream>

struct EnvironmentProperties
{
	bool IsSet = false;
	bool IsDrawingBackground = true;
	bool IsEnvironmentMapUsing = false;

	float Intensity = 1.f;
	float Rotation = 0.0f;
	float Blur = 0.0f;

	unsigned int CubemapTexture;
	unsigned int IrradianceMapTexture;
	unsigned int PrefilterMapTexture;
	unsigned int BRDFLUTTexture;

	glm::vec3 Color = glm::vec3(0.1f);
};

class EnvironmentMap
{
private:
	EnvironmentProperties m_Properties;
	
	unsigned int m_HDRITextureID; //Hdri texture

	unsigned int m_CaptureFBO, m_CaptureRBO;
	std::string m_Path;
	int m_Width, m_Height, m_BPP;
	int m_PrefilterSize;
	int m_OutputSize;

	VertexArray m_CubeVA;
	VertexArray m_QuadVA;

	Shader m_GenCubemapShader;
	Shader m_BackgroundShader;
	Shader m_IrradianceShader;
	Shader m_PrefilterShader;
	Shader m_BRDFShader;

	void Initialize(unsigned int size);
	void CreateCubemap(unsigned int& textureId, unsigned int size, bool createMipMap = false);
	void ConvertHDRIToCubemap();
	void BakeIrradianceMap();
	void Prefiltering();
	void BakeBRDF();
	void RenderToCubemap(Shader& shader, GLint sourceType, unsigned int& from, unsigned int& to, int& size);
	void RenderQuad();
	void RenderCube(const Shader& shader);

public:

	EnvironmentMap();
	EnvironmentMap(EnvironmentMap&) = default;
	EnvironmentMap(const char* path, int outputSize = 512, int prefilterSize = 128);

	void LoadEnvironmentMap(const char* path, int outputSize = 512, int prefilterSize = 128);

	void Draw(glm::mat4 view, glm::mat4 projection);

	void BindIBL(unsigned int irradianceSlot, unsigned int prefilterSlot, unsigned int brdfSlot);
	inline const unsigned int& GetEnvironmentMap() { return m_Properties.CubemapTexture; }
	inline const unsigned int& GetIrradianceMap() { return m_Properties.IrradianceMapTexture; }
	inline const unsigned int& GetHDRI() { return m_HDRITextureID; }
	inline EnvironmentProperties& GetProperties() { return m_Properties; }
	inline std::string& GetPath() { return m_Path; }
	inline glm::vec2 GetSize() { return glm::vec2(m_Width, m_Height); }
};