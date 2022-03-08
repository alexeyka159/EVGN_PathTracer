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

int main() {

	int WIDTH = 950, HEIGHT = 540;
	Renderer renderer(WIDTH, HEIGHT, "Path Tracer");
	
	EVGN::Time TIME;

	//float positions[] = {
	//	-1.0f, -1.0f,		0.0f, 0.0f, //0
	//	 1.0f, -1.0f,		1.0f, 0.0f, //1
	//	 1.0f,  1.0f,		1.0f, 1.0f, //2
	//	-1.0f,  1.0f,		0.0f, 1.0f  //3
	//};

	//unsigned int indices[] = {
	//	0, 1, 2,
	//	2, 3, 0
	//};

	glEnable(GL_BLEND);
	glBlendFunc(GL_SOURCE0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*VertexArray va;

	VertexBuffer vb(&positions[0], 4 * 4 * sizeof(float));
	VertexBufferLayout layout;

	layout.Push<float>(2);
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(&indices[0], 6);

	Shader screenShader("res/shaders/Framebuffer/fb1.vert", "res/shaders/Framebuffer/fb1.frag");*/

	Shader shader("res/shaders/sh1.vert", "res/shaders/sh1.frag");
	shader.Bind();

	/*Texture texture("res/textures/concrete.png");
	texture.Bind();
	shader.SetUniform1i("u_Texture", 0);*/

	/*va.Unbind();
	vb.Unbind();
	ib.Unbind();
	shader.Unbind();*/


	

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	glm::vec3 translationA(0, 0, 0);
	glm::vec3 translationB(-2.5f, 0, 0);

	float cameraSpeed = 5;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	CameraOrbit camera = CameraOrbit(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f), 45, cameraSpeed);
	//CameraFPS camera = CameraFPS(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f), 45, cameraSpeed);
	//glm::mat4 proj = glm::perspective(glm::radians(camera.GetFov()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view;

	InputManager inputManager(renderer.GetWindow());
	inputManager.Push(camera.GetController());

	bool show_debug_window = true;
	bool isWireframe = false;

	Scene activeScene;
	Entity monkey = activeScene.CreateEntity("Monkey");
	monkey.AddComponent<ModelRendererComponent>("res/models/monk_smooth.obj");
	Entity prims = activeScene.CreateEntity("Primitives");
	prims.AddComponent<ModelRendererComponent>("res/models/sphere and cube.obj");

	glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
	//Model testModel("res/models/sphere and cube.obj");
	//Model testModel1("res/models/monk_smooth.obj");

	FramebufferSpecification fbSpec;
	fbSpec.Width = WIDTH;
	fbSpec.Height = HEIGHT;
	Framebuffer frameBuffer(fbSpec);


	GUI gui(renderer.GetWindow());

	ViewportPanel* viewport = new ViewportPanel(frameBuffer, camera);
	gui.Push(viewport);

	SceneHierarchyPanel* outliner = new SceneHierarchyPanel(activeScene);
	gui.Push(outliner);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//rb.Bind();
	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		frameBuffer.Bind();
		glEnable(GL_DEPTH_TEST);

		TIME.UpdateTime();
		renderer.Clear();

		inputManager.ProcessInput();

		camera.GetController()->SetDeltaTime(TIME.DeltaTime());
		camera.SetSpeed(cameraSpeed);

		view = camera.GetViewMatrix();

		{
			glm::mat4 model = glm::translate(glm::mat4(1), translationA);
			glm::mat4 mvp = camera.GetProjection() * view * model;
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", mvp);
			shader.SetUniformMat4f("u_Model", model);

			if(monkey)
				monkey.GetComponent<ModelRendererComponent>().ModelObj.Draw(shader);
			
			//testModel.Draw(shader);
			//renderer.Draw(va, ib, shader);
		}

		{
			glm::mat4 model = glm::translate(glm::mat4(1), translationB);
			glm::mat4 mvp = camera.GetProjection() * view * model;
			shader.SetUniformMat4f("u_MVP", mvp);
			shader.SetUniformMat4f("u_Model", model);
			//testModel1.Draw(shader);
			if (prims)
				prims.GetComponent<ModelRendererComponent>().ModelObj.Draw(shader);
			//renderer.Draw(va, ib, shader);
		}

		{
			shader.SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
		}

		frameBuffer.Unbind();

		gui.Begin();

		if (show_debug_window)
		{
			ImGui::Begin("Debug Window", &show_debug_window);
			ImGui::SliderFloat3("Primitives Translation", &translationA.x, -8.0f, 8.f, "%f");
			ImGui::SliderFloat3("Monkey Translation", &translationB.x, -8.0f, 8.f, "%f");
			ImGui::SliderFloat3("Light Position", &lightPos.x, -5.0f, 5.f);
			ImGui::SliderFloat("Camera Speed", &cameraSpeed, 1.0f, 30.f);
			ImGui::Checkbox("Wireframe mode", &isWireframe);
			ImGui::Spacing();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		gui.Render();
		gui.End();

		inputManager.PollEvents();
		glfwSwapBuffers(renderer.GetWindow());
	}

	return 0;
}
