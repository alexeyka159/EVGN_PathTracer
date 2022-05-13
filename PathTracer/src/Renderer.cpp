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

void Renderer::Draw(Ref<Scene> scene, Shader& shader, Camera* camera, float ts) const
{
	if (scene)
	{

		auto pointLightView = scene->m_Registry.view<PointLightComponent>();
		size_t pointLightCounter = 0;
		for (auto entityID : pointLightView)
		{
			Entity lightEntity{ entityID, scene.get() };
			if (lightEntity)
			{
				auto& tc = lightEntity.GetComponent<TransformComponent>().Translation;
				auto& plc = lightEntity.GetComponent<PointLightComponent>();
				glm::vec3 lightColor = plc.Color * plc.Intensity;
				shader.Bind();
				shader.SetUniform3f("u_PointLights[" + std::to_string(pointLightCounter) + "].position",	tc.x, tc.y, tc.z);
				shader.SetUniform3f("u_PointLights[" + std::to_string(pointLightCounter) + "].color",		lightColor.r, lightColor.g, lightColor.b);
				shader.SetUniform1f("u_PointLights[" + std::to_string(pointLightCounter) + "].constant" ,	plc.Constant);
				shader.SetUniform1f("u_PointLights[" + std::to_string(pointLightCounter) + "].linear",		plc.Linear);
				shader.SetUniform1f("u_PointLights[" + std::to_string(pointLightCounter) + "].quadratic",	plc.Quadratic);
				shader.SetUniform1i("u_PointLightsInUse[" + std::to_string(pointLightCounter) + "]",	1);
				pointLightCounter++;
			}
		}

		auto spotLightView = scene->m_Registry.view<SpotLightComponent>();
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
				shader.SetUniform3f("u_SpotLight.position", tc.Translation.x, tc.Translation.y, tc.Translation.z);
				shader.SetUniform3f("u_SpotLight.direction", lightDirection.x, lightDirection.y, lightDirection.z);
				shader.SetUniform3f("u_SpotLight.color", lightColor.r, lightColor.g, lightColor.b);
				shader.SetUniform1f("u_SpotLight.constant", slc.Constant);
				shader.SetUniform1f("u_SpotLight.linear", slc.Linear);
				shader.SetUniform1f("u_SpotLight.quadratic", slc.Quadratic);
				shader.SetUniform1f("u_SpotLight.cutOff", slc.CutOff);
				shader.SetUniform1f("u_SpotLight.outerCutOff", slc.OuterCutOff);
				shader.SetUniform1i("u_SpotLightInUse", 1);
			}
		}

		auto dirLightView = scene->m_Registry.view<DirectionalLightComponent>();
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
				shader.SetUniform3f("u_DirLight.direction", lightDirection.x, lightDirection.y, lightDirection.z);
				shader.SetUniform3f("u_DirLight.color", lightColor.r, lightColor.g, lightColor.b);
				shader.SetUniform1i("u_DirLightInUse", 1);
			}
		}

		shader.Bind();
		shader.SetUniform3f("u_AmbientColor", 0.3f, 0.3f, 0.3f);

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

void RendererCallback::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
