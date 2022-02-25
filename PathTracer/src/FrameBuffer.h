#pragma once

#include "Renderer.h"

class FrameBuffer {
private:
	unsigned int m_RendererID;
	unsigned int m_Buffer;
	std::string m_Type;

	unsigned int m_Width, m_Height;

	void CreateFBTextureBuffer();
	void CreateFBRenderBuffer();
public:
	FrameBuffer(unsigned int width, unsigned int height, std::string type = "texture");
	~FrameBuffer();

	void BindTexture();

	void Bind();
	void Unbind();
};