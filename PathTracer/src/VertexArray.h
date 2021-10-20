#pragma once

#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray {
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();

	void bind() const;
	void unbind() const;

	void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);
};
