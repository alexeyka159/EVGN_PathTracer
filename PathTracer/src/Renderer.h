#pragma once

#include <glad/glad.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer {
public:
	void clear() const;
	void draw(const VertexArray& va, const IndexBuffer&  ib, const Shader& shader) const;
};
