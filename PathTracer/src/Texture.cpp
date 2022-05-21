#include "Texture.h"

#include "vendor/stb_image/stb_image.h"

Texture::Texture(const std::string& path, TextureType type)
	: m_RendererID(0)
	, m_FilePath(path)
	, m_Width(0)
	, m_Height(0)
	, m_BPP(0)
	, m_Type(type)
{
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 0);

	if(data) {
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, (m_Type == TextureType::DIFFUSE ? GL_SRGB_ALPHA : GL_RGBA8), m_Width, m_Height, 0, (m_BPP == 4 ? GL_RGBA : m_BPP == 3 ? GL_RGB : GL_RED), GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		stbi_image_free(data);
	}
	else {
		std::cout << "ERROR::TEXTURE::FAILED TO LOAD TEXTURE: \"" << path << "\"" << std::endl;
	}

	std::cout << "    - Loaded texture: \"" << m_FilePath << "\"" << std::endl;
		
}

Texture::~Texture()
{
	//glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::string TextureTypeConv::ConvertTypeToStr(Texture::TextureType type)
{
	switch (type)
	{
	case Texture::TextureType::DIFFUSE:
		return "texture_diffuse";
	case Texture::TextureType::METALIC:
		return "texture_metalic";
	case Texture::TextureType::ROUGHNESS:
		return "texture_roughness";
	case Texture::TextureType::NORMAL:
		return "texture_normal";
	case Texture::TextureType::EMISSION:
		return "texture_emission";
	case Texture::TextureType::ALPHA:
		return "texture_alpha";
	case Texture::TextureType::HEIGHT:
		return "texture_height";
	case Texture::TextureType::NON_COLOR:
		return "texture_noncolor";
	default:
		return "texture_emission";
		break;
	}
}

Texture::TextureType TextureTypeConv::ConvertStrToType(std::string type)
{
	if (type == "texture_diffuse")
		return Texture::TextureType::DIFFUSE;
	if (type == "texture_metalic")
		return Texture::TextureType::METALIC;
	if (type == "texture_roughness")
		return Texture::TextureType::ROUGHNESS;
	if (type == "texture_normal")
		return Texture::TextureType::NORMAL;
	if (type == "texture_emission")
		return Texture::TextureType::EMISSION;
	if (type == "texture_alpha")
		return Texture::TextureType::ALPHA;
	if (type == "texture_height")
		return Texture::TextureType::HEIGHT;
	if (type == "texture_noncolor")
		return Texture::TextureType::NON_COLOR;
	return Texture::TextureType::EMISSION;
}
