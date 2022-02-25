#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, std::string type)
	: m_RendererID(0)
	, m_Buffer(0)
	, m_Width(width)
	, m_Height(height)
	, m_Type(type)
{
	if (type == "texture")
		CreateFBTextureBuffer();
	else
		CreateFBRenderBuffer();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	//Unbind();
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_RendererID);
}

void FrameBuffer::BindTexture()
{
	glBindTexture(GL_TEXTURE_2D, m_Buffer);
}

void FrameBuffer::CreateFBTextureBuffer()
{
	glGenFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	
	//glViewport(m_Width, m_Height);

	glGenTextures(1, &m_Buffer);
	glBindTexture(GL_TEXTURE_2D, m_Buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	/*glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0,
		GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
	);*/

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Buffer, 0);

	/*if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not cemplete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}

void FrameBuffer::CreateFBRenderBuffer()
{
	glGenRenderbuffers(1, &m_Buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_Buffer);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);

	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Buffer);

	/*if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}

void FrameBuffer::Bind()
{
	if (m_Type == "texture")
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	else {
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Buffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
	}
}

void FrameBuffer::Unbind()
{
	if (m_Type == "texture")
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
