#pragma once

#include "Renderer.h"

#include <string>

class Texture {
	
private:
	std::string m_FilePath;
	int m_Width, m_Height, m_BPP;
	
public:
	unsigned int m_RendererID;
	enum class TextureType
	{
		DIFFUSE,
		METALIC,
		SPECULAR,
		NORMAL,
		EMISSION,
		ALPHA,
		HEIGHT,
		NON_COLOR
	};

	Texture(const std::string& path, TextureType type = TextureType::EMISSION);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline TextureType GetType() { return m_Type; }

	inline std::string GetPath() { return m_FilePath; }
	inline unsigned int GetId() { return m_RendererID; }

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

private:
	TextureType m_Type;
};

namespace TextureTypeConv
{
	std::string ConvertTypeToStr(Texture::TextureType type);
	Texture::TextureType ConvertStrToType(std::string type);
}