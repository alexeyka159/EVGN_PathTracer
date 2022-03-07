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
	uint32_t m_RendererID;
	uint32_t m_ColorAttachment, m_DepthAttachment;
public:
	Framebuffer(const FramebufferSpecification& spec);
	~Framebuffer();

	void Invalidate();

	void Bind();
	void Unbind();

	inline uint32_t GetColorAttachmentRendererId() const { return m_ColorAttachment; }
	inline const FramebufferSpecification& GetSpecification() const { return m_Specification; }
};
