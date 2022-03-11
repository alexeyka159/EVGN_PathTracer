#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Scene;
class Camera;
namespace RendererCallback {
	void framebuffer_size_callback(GLFWwindow*, int, int);
}

class Renderer {
private:
	int m_Width, m_Height;
	std::string m_WindowName;
	GLFWwindow* m_Window;

public:
	Renderer(int w, int h, std::string wndName);
	~Renderer();

	inline GLFWwindow* GetWindow() const { return m_Window;  }

	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, Camera& camera) const;
	void Draw(Scene& scene, Camera& camera) const;
};
