#include "Renderer.h"

#include "Scene/Entity.h"
#include "Camera/Camera.h"

Renderer::Renderer(int w, int h, std::string wndName)
	: m_Width(w)
	, m_Height(h)
	, m_WindowName(wndName)
	
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
}

Renderer::~Renderer()
{
	glfwTerminate();
}

void Renderer::Clear() const
{
	glClearColor(0.1f, 0.1, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();

	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	
}

void Renderer::Draw(Scene& scene, Camera& camera, Shader& shader) const
{
	// TODO: Сделать CameraComponent и получать камеру оттуда
	scene.m_Registry.each([&](auto entityID) {
		Entity entity{ entityID, &scene };
		if (entity)
		{
			glm::mat4 model = entity.GetComponent<TransformComponent>().GetTransform();
			glm::mat4 mvp = camera.GetProjection() * camera.GetViewMatrix() * model;
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", mvp);
			shader.SetUniformMat4f("u_Model", model);
			shader.SetUniform1i("u_EntityID", (int)entityID);
			glm::vec3 viewPos = camera.GetPosition();
			shader.SetUniform3f("u_ViewPos", viewPos.x, viewPos.y, viewPos.z);

			entity.GetComponent<ModelRendererComponent>().ModelObj.Draw(shader);
		}
	});
}

void RendererCallback::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
