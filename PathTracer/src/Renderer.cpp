#include "Renderer.h"

Renderer::Renderer(int w, int h, std::string wndName)
	: m_Width(w)
	, m_Height(h)
	, m_WindowName(wndName)
	
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), NULL, NULL);
	if (m_Window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		std::cin.get();
		throw std::exception("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(m_Window);
	glfwSetFramebufferSizeCallback(m_Window, RendererCallback::framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		std::cin.get();
		throw std::exception("Failed to initialize GLAD");
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();

	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	
}

void RendererCallback::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
