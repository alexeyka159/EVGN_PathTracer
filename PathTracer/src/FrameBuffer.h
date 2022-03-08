#pragma once
#include <cstdint>

struct FramebufferSpecification
{
	uint32_t Width, Height;
	//FramebufferFormat Format =  
	uint32_t Samples = 1;

	bool SwapChainTarget = false;
};

class Framebuffer
{
private:
	FramebufferSpecification m_Specification;
	uint32_t m_RendererID = 0;
	uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
public:
	Framebuffer(const FramebufferSpecification& spec);
	~Framebuffer();

	void Invalidate();

	void Bind();
	void Unbind();

	void Resize(uint32_t width, uint32_t height);

	inline uint32_t GetColorAttachmentRendererId() const { return m_ColorAttachment; }
	inline const FramebufferSpecification& GetSpecification() const { return m_Specification; }
};
