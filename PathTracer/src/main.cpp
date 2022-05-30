#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

#include "Shader.h"

#include "Texture.h"

#include "GUI/GUI.h"
#include "GUI/Panels/ViewportPanel.h"
#include "GUI/Panels/SceneHierarchyPanel.h"

#include "InputManager.h"
#include "Camera/Camera.h"
#include "Camera/CameraFPS.h"
#include "Camera/CameraOrbit.h"

#include "EVGNTime.h"

#include "Model/Model.h"

#include "Framebuffer.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/SceneSerializer.h"

#include "EnvironmentMap.h"

#include "Ref.h"

int main() {

	int WIDTH = 1280, HEIGHT = 720;
	Renderer renderer(WIDTH, HEIGHT, "Evergreen Engine");
	
	EVGN::Time TIME;


	float gridPositions[] = {
		-1.0f, -1.0f,		0.0f, 0.0f, //0
		 1.0f, -1.0f,		1.0f, 0.0f, //1
		 1.0f,  1.0f,		1.0f, 1.0f, //2
		-1.0f,  1.0f,		0.0f, 1.0f  //3
	};
	glm::mat4 gridModel = glm::mat4(1);
	{
		glm::mat4 gridTr = glm::mat4(1);
		gridTr = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
		glm::mat4 gridRot = glm::mat4(1);
		gridRot = glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(-1.0f, 0.0f, 0.0f));
		glm::mat4 gridSc = glm::mat4(1);
		gridSc = glm::scale(gridSc, glm::vec3(100));
		gridModel = glm::mat4(1) * gridRot * gridSc;
	}

	unsigned int gridIndices[] = {
		0, 1, 2,
		2, 3, 0
	};
	
	VertexArray gridVA;

	VertexBuffer gridVB(gridPositions, 4 * 4 * sizeof(float));
	VertexBufferLayout gridLayout;

	gridLayout.Push<float>(2);
	gridLayout.Push<float>(2);
	gridVA.AddBuffer(gridVB, gridLayout);

	IndexBuffer gridIB(gridIndices, 6);
	gridVA.Unbind();
	gridVB.Unbind();
	gridIB.Unbind();



	Shader shader("res/shaders/sh1.vert", "res/shaders/sh1.frag");
	Shader pbrShader("res/shaders/pbr.vert", "res/shaders/pbr.frag");
	Shader gridShader("res/shaders/floor.vert", "res/shaders/floor.frag");
	//pbrShader.Bind();

	float cameraSpeed = 5;
	glm::vec3 cameraPos = glm::vec3(-4.3f, 3.0f, 5.3f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	CameraOrbit camera = CameraOrbit(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f), 45, cameraSpeed);
	//CameraFPS camera = CameraFPS(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f), 45, cameraSpeed);
	glm::mat4 view;

	InputManager inputManager(renderer.GetWindow());
	inputManager.Push(camera.GetController());

	bool show_debug_window = false;
	bool isDebug = false;

	Ref<Scene> activeScene = std::make_shared<Scene>();

	//glm::vec3 lightPos(5.0f, 5.0f, 5.0f);

	FramebufferSpecification fbSpec;
	fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
	fbSpec.Width = WIDTH;
	fbSpec.Height = HEIGHT;
	Framebuffer frameBuffer(fbSpec);


	GUI gui(renderer.GetWindow());

	SceneHierarchyPanel* outliner = new SceneHierarchyPanel(activeScene, *renderer.GetWindow());
	gui.Push(outliner);

	ViewportPanel* viewport = new ViewportPanel(frameBuffer, *renderer.GetWindow(), *outliner, camera);
	gui.Push(viewport);

	//inputManager.Push(viewport->GetController());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//SceneSerializer serializer(activeScene);
	//serializer.Serialize("res/assets/scene/test.evgn");
	//serializer.Deserialize("res/assets/scene/test.evgn");

	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		frameBuffer.Bind();
		glEnable(GL_DEPTH_TEST);

		TIME.UpdateTime();
		renderer.Clear();
		frameBuffer.ClearAttachment(1, -1);


		inputManager.ProcessInput();

		camera.GetController()->SetDeltaTime(TIME.DeltaTime());

		//Если энвайренмент карта загружена
		if (outliner->GetEnvironment()->GetProperties().IsSet) { 
			Ref<EnvironmentMap> environment = outliner->GetEnvironment();
			
			//Если карта влияет на освещение
			pbrShader.Bind();
			pbrShader.SetUniform1i("u_Environment.isEnvironmentMapUsing", environment->GetProperties().IsEnvironmentMapUsing);
			if (environment->GetProperties().IsEnvironmentMapUsing)
			{
				pbrShader.SetUniform1f("u_Environment.intensity", environment->GetProperties().Intensity);
				environment->BindIBL(10, 11, 12);
				pbrShader.SetUniform1i("u_Environment.irradianceMap", 10);
				pbrShader.SetUniform1i("u_Environment.prefilterMap", 11);
				pbrShader.SetUniform1i("u_Environment.brdfLUT", 12);
				glm::mat4 rotation = glm::mat4(1);
				rotation = glm::rotate(rotation, glm::radians(environment->GetProperties().Rotation), glm::vec3(0, 1, 0));
				pbrShader.SetUniformMat4f("u_Environment.rotation", rotation);
			}
			else
			{
				pbrShader.SetUniformVec3f("u_Environment.color", outliner->GetEnvironment()->GetProperties().Color);
			}
		}
		else if(!outliner->GetEnvironment()->GetProperties().IsEnvironmentMapUsing)
		{
			pbrShader.Bind();
			pbrShader.SetUniform1i("u_Environment.isEnvironmentMapUsing", false);
			//pbrShader.SetUniformVec3f("u_Environment.color", outliner->GetEnvironment()->GetProperties().Color);
		}
		renderer.Draw(*outliner->GetEnvironment().get(), &camera);
		
		renderer.Draw(outliner->GetContex(), pbrShader, &camera, TIME.DeltaTime());
		

		{
			gridShader.Bind();
			gridShader.SetUniformMat4f("u_View", camera.GetViewMatrix());
			gridShader.SetUniformMat4f("u_Proj", camera.GetProjection());
			gridShader.SetUniformMat4f("u_Model", gridModel);
			gridShader.Unbind();
			renderer.FaceCulling(false);
			renderer.Draw(gridVA, gridIB, gridShader);
			renderer.FaceCulling(true);
		}

		frameBuffer.Unbind();

		gui.Begin();
		gui.Render();
		gui.End();

		inputManager.PollEvents();
		glfwSwapBuffers(renderer.GetWindow());
	}

	return 0;
}
