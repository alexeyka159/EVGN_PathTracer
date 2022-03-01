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

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "InputManager.h"
#include "Camera/Camera.h"
#include "Camera/CameraFPS.h"
#include "Camera/CameraOrbit.h"

#include "EVGNTime.h"

#include "Model/Model.h"

#include "FrameBuffer.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

int main() {

	int WIDTH = 950, HEIGHT = 540;
	Renderer renderer(950, 540, "Path Tracer");
	
	EVGN::Time TIME;
	//std::cout << __cplusplus << std::endl;
	float positions[] = {
		-1.0f, -1.0f,		0.0f, 0.0f, //0
		 1.0f, -1.0f,		1.0f, 0.0f, //1
		 1.0f,  1.0f,		1.0f, 1.0f, //2
		-1.0f,  1.0f,		0.0f, 1.0f  //3
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SOURCE0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	VertexArray va;

	VertexBuffer vb(&positions[0], 4 * 4 * sizeof(float));
	VertexBufferLayout layout;

	layout.Push<float>(2);
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(&indices[0], 6);

	Shader screenShader("res/shaders/Framebuffer/fb1.vert", "res/shaders/Framebuffer/fb1.frag");
	Shader shader("res/shaders/sh1.vert", "res/shaders/sh1.frag");
	shader.Bind();

	/*Texture texture("res/textures/concrete.png");
	texture.Bind();
	shader.SetUniform1i("u_Texture", 0);*/

	va.Unbind();
	vb.Unbind();
	ib.Unbind();
	shader.Unbind();


	const char* glsl_version = "#version 330";
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(renderer.GetWindow(), true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();

	glm::vec3 translationA(0, 0, 0);
	glm::vec3 translationB(-2.5f, 0, 0);

	float cameraSpeed = 5;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	CameraOrbit camera = CameraOrbit(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f), 45, cameraSpeed);
	//CameraFPS camera = CameraFPS(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f), 45, cameraSpeed);

	glm::mat4 proj = glm::perspective(glm::radians(camera.GetFov()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view;

	InputManager inputManager(renderer.GetWindow());
	inputManager.Push(camera.GetController());

	bool show_debug_window = true;
	bool isWireframe = false;



	Scene activeScene;
	Entity monkey = activeScene.CreateEntity("Monkey");
	monkey.AddComponent<ModelRendererComponent>("res/models/monk_smooth.obj");




	glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
	//Model testModel("res/models/sphere and cube.obj");
	//Model testModel1("res/models/monk_smooth.obj");

	FrameBuffer fb(WIDTH, HEIGHT, "texture");
	FrameBuffer rb(WIDTH, HEIGHT, "render");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	rb.Bind();
	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		fb.Bind();
		glEnable(GL_DEPTH_TEST);

		TIME.UpdateTime();
		renderer.Clear();

		inputManager.ProcessInput();

		camera.GetController()->SetDeltaTime(TIME.DeltaTime());
		camera.SetSpeed(cameraSpeed);

		view = camera.GetViewMatrix();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		{
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New", "Ctrl N"))
					{
						//Do something
					}
					if (ImGui::MenuItem("Open", "Ctrl O"))
					{
						//Do something
					}
					if (ImGui::MenuItem("Save", "Ctrl S"))
					{
						//Do something
					}
					if (ImGui::MenuItem("Save As", "Ctrl Shift S"))
					{
						//Do something
					}
					if (ImGui::MenuItem("Import"))
					{
						//Do something
					}
					if (ImGui::MenuItem("Export"))
					{
						//Do something
					}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Edit"))
			{
			}
			if (ImGui::MenuItem("Add"))
			{
			}
			if (ImGui::MenuItem("Selection"))
			{
			}
			ImGui::EndMainMenuBar();
			}
		}


		{
			glm::mat4 model = glm::translate(glm::mat4(1), translationA);
			glm::mat4 mvp = proj * view * model;
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
			glm::mat4 mvp = proj * view * model;
			shader.SetUniformMat4f("u_MVP", mvp);
			shader.SetUniformMat4f("u_Model", model);
			//testModel1.Draw(shader);

			//renderer.Draw(va, ib, shader);
		}

		{
			shader.SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
		}

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

		

		fb.Unbind();
		renderer.Clear();
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		{
			screenShader.Bind();
			fb.BindTexture();
			renderer.Draw(va, ib, screenShader);
		}

		if (isWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		inputManager.PollEvents();
		glfwSwapBuffers(renderer.GetWindow());
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	glfwTerminate();
	return 0;
}
