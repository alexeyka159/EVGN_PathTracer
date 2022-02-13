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

void framebuffer_size_callback(GLFWwindow*, int, int);

int main() {

	int WIDTH = 950, HEIGHT = 540;
	
	EVGN::Time TIME;
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Path Tracer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		std::cin.get();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		std::cin.get();
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
	
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

	Shader shader("res/shaders/sh1.vert", "res/shaders/sh1.frag");
	shader.Bind();

	Texture texture("res/textures/concrete.png");
	texture.Bind();
	shader.SetUniform1i("u_Texture", 0);

	va.Unbind();
	vb.Unbind();
	ib.Unbind();
	shader.Unbind();

	Renderer renderer;

	const char* glsl_version = "#version 330";
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();

	glm::vec3 translationA(0, 0, 0);
	glm::vec3 translationB(1.5f, 0, 1.5);

	//Создание камеры
	//Type - enum Camera::orbital или Camera::fps
	/*Camera camera(type, pos, dir, fov);

	//Изменение камеры в лупе по инпуту
	//Геттер на получение матрицы. Внутри считывается инпут в CameraController
	glm::mat4 view = camera.GetViewMatrix(window);
	glm::mat4 mvp = proj * view * model;
	shader.Bind();
	shader.SetUniformMat4f("u_MVP", mvp);

	//процессинг инпутов методов в лупе
	Controller(vector<вектор всех методов на обработку>);*/


	float cameraSpeed = 5;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	CameraOrbit camera = CameraOrbit(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f), 45, cameraSpeed);

	glm::mat4 proj = glm::perspective(glm::radians(camera.GetFov()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 view;

	InputManager inputManager(window);
	inputManager.Push(camera.GetController());

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	bool show_debug_window = true;
	bool isWireframe = false;

	glm::vec3 lightPos(3.0f, 3.0f, 3.0f);
	Model testModel("res/models/monk_smooth.obj");
	while (!glfwWindowShouldClose(window))
	{
		TIME.UpdateTime();
		renderer.Clear();

		inputManager.ProcessInput();

		camera.GetController()->SetDeltaTime(TIME.DeltaTime());
		camera.SetSpeed(cameraSpeed);

		/*Апдейт шейдеров*/
		view = camera.GetViewMatrix();


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		/*{
			ImGui::BeginMainMenuBar();
			if (ImGui::MenuItem("File"))
			{
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
		}*/

		{
			glm::mat4 model = glm::translate(glm::mat4(1), translationA);
			glm::mat4 mvp = proj * view * model;
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", mvp);
			shader.SetUniformMat4f("u_Model", model);

			testModel.Draw(shader);

			//renderer.Draw(va, ib, shader);
		}

		/*{
			glm::mat4 model = glm::translate(glm::mat4(1), translationB);
			glm::mat4 mvp = proj * view * model;
			shader.SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(va, ib, shader);
		}*/

		{
			shader.SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
		}

		if (show_debug_window)
		{
			ImGui::Begin("Debug Window", &show_debug_window);
			ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 10.f);
			//ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 10.f);
			ImGui::SliderFloat3("Light position", &lightPos.x, -3.0f, 3.f);
			ImGui::SliderFloat("Camera Speed", &cameraSpeed, 1.0f, 30.f);
			ImGui::Checkbox("Wireframe mode", &isWireframe);
			ImGui::Spacing();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		if (isWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		inputManager.PollEvents();
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
