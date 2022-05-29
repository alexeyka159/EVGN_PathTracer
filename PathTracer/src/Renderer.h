#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#include "EnvironmentMap.h"

#include <Ref.h>

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
	void FaceCulling(bool state);
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void Draw(const VertexArray& va, const unsigned int& verticesCount, const Shader& shader) const;
	void Draw(EnvironmentMap& envir, Camera* camera);
	void Draw(Ref<Scene> scene, Shader& shader, Camera* camera, float ts) const;
};
