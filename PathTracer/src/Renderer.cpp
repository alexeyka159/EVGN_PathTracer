#include "Renderer.h"

#include "Scene/Entity.h"
#include "Camera/CameraOrbit.h"
#include "Camera/CameraController.h"


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

	std::cout << glGetString(GL_VERSION) << std::endl << std::endl;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	FaceCulling(true);
}

Renderer::~Renderer()
{
	glfwTerminate();
}

void Renderer::FaceCulling(bool state)
{
	if(state)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
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

	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
	va.Unbind();
	shader.Unbind();
}

void Renderer::Draw(const VertexArray& va, const unsigned int& verticesCount, const Shader& shader) const
{
	shader.Bind();
	va.Bind();

	glDrawArrays(GL_TRIANGLES, 0, verticesCount);

	va.Unbind();
	shader.Unbind();
}

void Renderer::Draw(EnvironmentMap& envir, Camera* camera)
{
	envir.Draw(camera->GetViewMatrix(), camera->GetProjection());
}

void Renderer::Draw(Ref<Scene> scene, Shader& shader, Camera* camera, float ts) const
{
	if (scene)
	{

		auto pointLightView = scene->m_Registry.view<PointLightComponent>();
		size_t lightCounter = 0;
		for (auto entityID : pointLightView)
		{
			Entity lightEntity{ entityID, scene.get() };
			if (lightEntity)
			{
				auto& tc = lightEntity.GetComponent<TransformComponent>().Translation;
				auto& plc = lightEntity.GetComponent<PointLightComponent>();
				glm::vec3 lightColor = plc.Color * plc.Intensity;
				shader.Bind();
				shader.SetUniform3f("u_PointLights[" + std::to_string(lightCounter) + "].position",	tc.x, tc.y, tc.z);
				shader.SetUniform3f("u_PointLights[" + std::to_string(lightCounter) + "].color",		lightColor.r, lightColor.g, lightColor.b);

				shader.SetUniform1i("u_PointLightsInUse[" + std::to_string(lightCounter) + "]",	1);
				lightCounter++;
			}
		}

		auto spotLightView = scene->m_Registry.view<SpotLightComponent>();
		lightCounter = 0;
		for (auto entityID : spotLightView)
		{
			Entity lightEntity{ entityID, scene.get() };
			if (lightEntity)
			{
				auto& tc = lightEntity.GetComponent<TransformComponent>();
				auto& slc = lightEntity.GetComponent<SpotLightComponent>();

				glm::vec3 lightColor = slc.Color * slc.Intensity;
				glm::vec3 lightDirection;

				glm::mat4 rotation = glm::toMat4(glm::quat(tc.Rotation));
				lightDirection = rotation * glm::vec4(0, 1, 0, 1);

				shader.Bind();
				shader.SetUniform3f("u_SpotLights[" + std::to_string(lightCounter) + "].position", tc.Translation.x, tc.Translation.y, tc.Translation.z);
				shader.SetUniform3f("u_SpotLights[" + std::to_string(lightCounter) + "].direction", lightDirection.x, lightDirection.y, lightDirection.z);
				shader.SetUniform3f("u_SpotLights[" + std::to_string(lightCounter) + "].color", lightColor.r, lightColor.g, lightColor.b);

				shader.SetUniform1f("u_SpotLights[" + std::to_string(lightCounter) + "].cutOff", slc.CutOff);
				shader.SetUniform1f("u_SpotLights[" + std::to_string(lightCounter) + "].outerCutOff", slc.OuterCutOff);
				shader.SetUniform1i("u_SpotLightsInUse[" + std::to_string(lightCounter) + "]", 1);
				lightCounter++;
			}
		}

		auto dirLightView = scene->m_Registry.view<DirectionalLightComponent>();
		lightCounter = 0;
		for (auto entityID : dirLightView)
		{
			Entity lightEntity{ entityID, scene.get() };
			if (lightEntity)
			{
				auto& tc = lightEntity.GetComponent<TransformComponent>();
				auto& dlc = lightEntity.GetComponent<DirectionalLightComponent>();

				glm::vec3 lightColor = dlc.Color * dlc.Intensity;
				glm::vec3 lightDirection;

				glm::mat4 rotation = glm::toMat4(glm::quat(tc.Rotation));
				lightDirection = rotation * glm::vec4(0, -1, 0, 1);

				shader.Bind();
				shader.SetUniform3f("u_DirLights[" + std::to_string(lightCounter) +"].direction", lightDirection.x, lightDirection.y, lightDirection.z);
				shader.SetUniform3f("u_DirLights[" + std::to_string(lightCounter) +"].color", lightColor.r, lightColor.g, lightColor.b);
				shader.SetUniform1i("u_DirLightsInUse[" + std::to_string(lightCounter) + "]", 1);
				lightCounter++;
			}
		}

		shader.Bind();
		shader.SetUniform3f("u_Environment.color", 0.1f, 0.1f, 0.1f);

		auto view = scene->m_Registry.view<ModelRendererComponent>();
		for (auto entityID : view)
		{
			Entity entity{ entityID, scene.get() };
			if (entity)
			{
				glm::mat4 model = entity.GetComponent<TransformComponent>().GetTransform();
				glm::mat4 mvp = camera->GetProjection() * camera->GetViewMatrix() * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				shader.SetUniformMat4f("u_Model", model);
				shader.SetUniform1i("u_EntityID", (int)entityID);
				glm::vec3 viewPos = camera->GetPosition();
				shader.SetUniform3f("u_ViewPos", viewPos.x, viewPos.y, viewPos.z);

				entity.GetComponent<ModelRendererComponent>().ModelObj.Draw(shader);
			}
		}
	}
}

//void Renderer::RenderToFile(const char* path)
//{
//		int width, height;
//		glfwGetFramebufferSize(m_Window, &width, &height);
//		GLsizei nrChannels = 3;
//		GLsizei stride = nrChannels * width;
//		stride += (stride % 4) ? (4 - stride % 4) : 0;
//		GLsizei bufferSize = stride * height;
//		std::vector<char> buffer(bufferSize);
//		glPixelStorei(GL_PACK_ALIGNMENT, 4);
//		glReadBuffer(GL_FRONT);
//		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
//		stbi_flip_vertically_on_write(true);
//		stbi_write_png(path, width, height, nrChannels, buffer.data(), stride);
//}

void RendererCallback::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
