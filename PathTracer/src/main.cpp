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

#include "Physics/Gravity.h"

bool is_update = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		is_update = !is_update;
}

int main() {

	int WIDTH = 950, HEIGHT = 540;
	Renderer renderer(WIDTH, HEIGHT, "Gravity Simulation");
	
	EVGN::Time TIME;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SOURCE0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	float cameraSpeed = 5;
	glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 25.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	CameraOrbit camera = CameraOrbit(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f), 45, cameraSpeed);
	//CameraFPS camera = CameraFPS(cameraPos, cameraTarget, glm::vec3(0.f, 1.f, 0.f), 45, cameraSpeed);
	glm::mat4 view;

	InputManager inputManager(renderer.GetWindow());
	inputManager.Push(camera.GetController());

	bool show_debug_window = false;


	Scene activeScene;
	
	Shader sunShader("res/shaders/sun.vert", "res/shaders/sun.frag");
	Shader asteroidShader("res/shaders/sh1.vert", "res/shaders/sh1.frag");
	
		Model asteroidModel("res/models/Asteroid.obj", asteroidShader);

		Entity sun = activeScene.CreateEntity("Sun");
		sun.AddComponent<ModelRendererComponent>(Model("res/models/Sun.obj", sunShader));
		sun.AddComponent<GravityComponent>(100, 50, glm::vec3(0.0f, 0.0f, 0.0f));

		Entity asteroid1 = activeScene.CreateEntity("Asteroid1");
		asteroid1.AddComponent<ModelRendererComponent>(asteroidModel);
		asteroid1.GetComponent<TransformComponent>().Transform = glm::translate(asteroid1.GetComponent<TransformComponent>().Transform,
			glm::vec3(15.0f, 0.0f, 0.0f));
		asteroid1.AddComponent<GravityComponent>(2, 5, glm::vec3(0, 0.0f, 2.1f));

		Entity asteroid2 = activeScene.CreateEntity("Asteroid2");
		asteroid2.AddComponent<ModelRendererComponent>(asteroidModel);
		asteroid2.GetComponent<TransformComponent>().Transform = glm::translate(asteroid2.GetComponent<TransformComponent>().Transform,
			glm::vec3(26.0f, 0.0f, 0.0f));
		asteroid2.AddComponent<GravityComponent>(10.f, 5, glm::vec3(0, 0.0f, -1.85f));

		Entity asteroid3 = activeScene.CreateEntity("Asteroid3");
		asteroid3.AddComponent<ModelRendererComponent>(asteroidModel);
		asteroid3.GetComponent<TransformComponent>().Transform = glm::translate(asteroid3.GetComponent<TransformComponent>().Transform,
			glm::vec3(30.0f, 0.0f, 0.0f));
		asteroid3.AddComponent<GravityComponent>(1.f, .1f, glm::vec3(0, 0.0f, -0.3f));
	

	Gravity gravity;

	glm::vec3 lightPos(0.0f, .0f, .0f);

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

	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		frameBuffer.Bind();
		glEnable(GL_DEPTH_TEST);

		TIME.UpdateTime();
		renderer.Clear();

		glfwSetKeyCallback(renderer.GetWindow(), key_callback);

		lightPos = {
			sun.GetComponent<TransformComponent>().Transform[0][0],
			sun.GetComponent<TransformComponent>().Transform[1][1],
			sun.GetComponent<TransformComponent>().Transform[2][2] };

		if(is_update)
			gravity.Update(activeScene, TIME.DeltaTime());

		inputManager.ProcessInput();

		camera.GetController()->SetDeltaTime(TIME.DeltaTime());
		camera.SetSpeed(cameraSpeed);

		renderer.Draw(activeScene, camera);

		{
			asteroidShader.Bind();
			asteroidShader.SetUniform3f("u_LightPos", lightPos.x, lightPos.y, lightPos.z);
			asteroidShader.Unbind();
		}

		frameBuffer.Unbind();

		gui.Begin();

		//if (show_debug_window)
		//{
		//	ImGui::Begin("Debug Window", &show_debug_window);
		//	//ImGui::SliderFloat3("Primitives Translation", &translationA.x, -8.0f, 8.f, "%f");
		//	//ImGui::SliderFloat3("Monkey Translation", &translationB.x, -8.0f, 8.f, "%f");
		//	ImGui::SliderFloat3("Light Position", &lightPos.x, -5.0f, 5.f);
		//	ImGui::SliderFloat("Camera Speed", &cameraSpeed, 1.0f, 30.f);
		//	ImGui::Checkbox("Wireframe mode", &isWireframe);
		//	ImGui::Spacing();
		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//	ImGui::End();
		//}

		gui.Render();
		gui.End();

		inputManager.PollEvents();
		glfwSwapBuffers(renderer.GetWindow());
	}

	return 0;
}
