#include "Renderer.h"

#include "Scene/Entity.h"
#include "Camera/Camera.h"

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, Camera& camera) const
{
	//glm::mat4 mvp = camera.GetProjection() * camera.GetViewMatrix() * model;

	shader.Bind();
	va.Bind();
	ib.Bind();

	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	
}

void Renderer::Draw(Scene& scene, Camera& camera, float timeStep = 0.0f) const
{
	// TODO: ������� CameraComponent � �������� ������ ������
	// TODO: ����������� �� ��� � ������ ��� ������ � ����������������������
	scene.m_Registry.each([&](auto entityID) {
		Entity entity{ entityID, &scene };
		if (entity.HasComponent<ModelRendererComponent>())
		{
			glm::mat4 model = entity.GetComponent<TransformComponent>().GetTransform();
			glm::mat4 mvp = camera.GetProjection() * camera.GetViewMatrix() * model;
			Model& renderModel = entity.GetComponent<ModelRendererComponent>().ModelObj;
			Shader& shader = renderModel.GetShader();
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", mvp);
			shader.SetUniformMat4f("u_Model", model);

			renderModel.Draw();

			/*������ �����*/
			if (entity.HasComponent<TrailComponent>())
			{
				glDisable(GL_CULL_FACE);
				auto& trailComponent = entity.GetComponent<TrailComponent>();
				//float timer = trailComponent.Timer;
				std::vector<glm::vec3> pos = trailComponent.Positions;
				if (pos.size())
				{
					VertexArray va;

					std::vector <glm::vec4> vData;
					for (int i = 0; i < pos.size(); i++)
						vData.push_back(glm::vec4(pos[i], (float)i));
					VertexBuffer vb(&vData[0], vData.size() * 4 * sizeof(float));
					VertexBufferLayout layout;

					layout.Push<float>(4);
					va.AddBuffer(vb, layout);

					trailComponent.TrailShader->Bind();
					trailComponent.TrailShader->SetUniformMat4f("u_VP", (camera.GetProjection() * camera.GetViewMatrix()));
					trailComponent.TrailShader->SetUniform1f("u_Size", (float)pos.size());
					glm::vec3 color = trailComponent.Color;
					trailComponent.TrailShader->SetUniform3f("u_Color", color.r, color.g, color.b);
					trailComponent.TrailShader->SetUniform1f("u_DotSize", 7);

					glDrawArrays(GL_POINTS, 0, pos.size());
					trailComponent.TrailShader->Unbind();

					va.Unbind();
					vb.Unbind();
				}
				glEnable(GL_CULL_FACE);
				//entity.GetComponent<TrailComponent>().Timer = timer;
			}

		}
	});
}

void RendererCallback::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
