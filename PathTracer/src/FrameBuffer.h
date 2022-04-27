#pragma once
#include <cstdint>
#include <vector>

enum class FramebufferTextureFormat {
	None = 0,

	//Color
	RGBA8,

	//Depth/stencil
	DEPTH24STENCIL8,

	//Defaults
	Depth = DEPTH24STENCIL8
};

struct FramebufferTextureSpecification
{
	FramebufferTextureSpecification() = default;
	FramebufferTextureSpecification(FramebufferTextureFormat format)
		: TexturetFormat(format) {}

	FramebufferTextureFormat TexturetFormat;
};

struct FramebufferAttachmentSpecification
{
	FramebufferAttachmentSpecification() = default;
	FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
		: Attachments(attachments) {}

	FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;

	std::vector<FramebufferTextureSpecification> Attachments;
};

struct FramebufferSpecification
{
	uint32_t Width, Height;
	FramebufferAttachmentSpecification Attachments;
	uint32_t Samples = 1;

	bool SwapChainTarget = false;
};

class Framebuffer
{
private:
	FramebufferSpecification m_Specification;
	uint32_t m_RendererID = 0;

	std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
	FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

	std::vector<uint32_t> m_ColorAttachments;
	uint32_t m_DepthAttachment = 0;
public:
	Framebuffer(const FramebufferSpecification& spec);
	~Framebuffer();

	void Invalidate();

	void Bind();
	void Unbind();

	void Resize(uint32_t width, uint32_t height);

	inline uint32_t GetColorAttachmentRendererId(uint32_t index = 0) const { return m_ColorAttachments[index]; }
	inline const FramebufferSpecification& GetSpecification() const { return m_Specification; }
};
