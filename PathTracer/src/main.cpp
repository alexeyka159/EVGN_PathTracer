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

	InputManager inputManager(renderer.GetWindow());
	inputManager.Push(camera.GetController());

	bool show_debug_window = false;


	Scene activeScene;
	
	Shader sunShader("res/shaders/sun.vert", "res/shaders/sun.frag");
	Shader asteroidShader("res/shaders/sh1.vert", "res/shaders/sh1.frag");
	Shader trailShader("res/shaders/trail.vert", "res/shaders/trail.frag", "res/shaders/trail.geom");
	
		Model asteroidModel("res/models/Asteroid.obj", asteroidShader);


		Entity sun = activeScene.CreateEntity("Sun");
		sun.AddComponent<ModelRendererComponent>(Model("res/models/Sun.obj", sunShader));
		sun.GetComponent<TransformComponent>().Translation += glm::vec3(0, 1.5f, 0);
		sun.AddComponent<GravityComponent>(100, 5, glm::vec3(0.0f, 0.0f, 0.0f));
		sun.GetComponent<TransformComponent>().Scale = glm::vec3(sun.GetComponent<GravityComponent>().Radius / 2);
		sun.AddComponent<TrailComponent>(&trailShader);

		Entity earth = activeScene.CreateEntity("Earth");
		earth.AddComponent<ModelRendererComponent>("res/models/earth.obj", asteroidShader);
		earth.GetComponent<TransformComponent>().Translation += glm::vec3(26.0f, 0.0f, 0.0f);
		earth.AddComponent<GravityComponent>(10.f, 1, glm::vec3(0, 0.0f, -1.85f));
		earth.GetComponent<TransformComponent>().Scale = glm::vec3(earth.GetComponent<GravityComponent>().Radius/2);
		earth.AddComponent<TrailComponent>(&trailShader);

		Entity moon = activeScene.CreateEntity("Moon");
		moon.AddComponent<ModelRendererComponent>(asteroidModel);
		moon.GetComponent<TransformComponent>().Translation += glm::vec3(30.0f, 0.0f, 0.0f);
		moon.AddComponent<GravityComponent>(1.f, .3f, glm::vec3(0, 0.0f, -.3f));
		moon.GetComponent<TransformComponent>().Scale = glm::vec3(moon.GetComponent<GravityComponent>().Radius / 2);
		moon.AddComponent<TrailComponent>(&trailShader);

		/*Entity asteroid4 = activeScene.CreateEntity("Asteroid4");
		asteroid4.AddComponent<ModelRendererComponent>(asteroidModel);
		asteroid4.GetComponent<TransformComponent>().Translation += glm::vec3(35.0f, 0.0f, 0.0f);
		asteroid4.AddComponent<GravityComponent>(5, 5, glm::vec3(0, 0.0f, 1.8f));
		asteroid4.GetComponent<TransformComponent>().Scale = glm::vec3(asteroid4.GetComponent<GravityComponent>().Radius / 2);
		asteroid4.AddComponent<TrailComponent>(&trailShader);

		Entity asteroid5 = activeScene.CreateEntity("Asteroid5");
		asteroid5.AddComponent<ModelRendererComponent>(asteroidModel);
		asteroid5.GetComponent<TransformComponent>().Translation += glm::vec3(39.0f, 0.0f, 0.0f);
		asteroid5.AddComponent<GravityComponent>(.1f, 5, glm::vec3(0, 0.0f, 0.5f));
		asteroid5.GetComponent<TransformComponent>().Scale = glm::vec3(asteroid5.GetComponent<GravityComponent>().Radius / 2);
		asteroid5.AddComponent<TrailComponent>(&trailShader);

		Entity asteroid6 = activeScene.CreateEntity("Saturn");
		asteroid6.AddComponent<ModelRendererComponent>("res/models/Saturn.obj", asteroidShader);
		asteroid6.GetComponent<TransformComponent>().Translation += glm::vec3(50.0f, 0.0f, 0.0f);
		asteroid6.GetComponent<TransformComponent>().Scale = glm::vec3(3, 3, 3);
		asteroid6.AddComponent<GravityComponent>(3, 5, glm::vec3(0, 0.0f, 1.3f));
		asteroid6.GetComponent<TransformComponent>().Scale = glm::vec3(asteroid6.GetComponent<GravityComponent>().Radius / 2);
		asteroid6.AddComponent<TrailComponent>(&trailShader);*/
	

	Gravity gravity;

	glm::vec3 lightPos(0.0f, 3.0f, 0.f);

	FramebufferSpecification fbSpec;
	fbSpec.Width = WIDTH;
	fbSpec.Height = HEIGHT;
	Framebuffer frameBuffer(fbSpec);


	GUI gui(renderer.GetWindow());

	ViewportPanel* viewport = new ViewportPanel(frameBuffer, camera/*, activeScene, asteroidModel, trailShader, renderer.GetWindow()*/);
	gui.Push(viewport);

	SceneHierarchyPanel* outliner = new SceneHierarchyPanel(activeScene, camera, gravity.simulationSpeed);
	gui.Push(outliner);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		frameBuffer.Bind();
		glEnable(GL_DEPTH_TEST);

		TIME.UpdateTime();
		renderer.Clear();

		glfwSetKeyCallback(renderer.GetWindow(), key_callback);

		if(sun)
			lightPos = sun.GetComponent<TransformComponent>().Translation;

		if(is_update)
			gravity.Update(activeScene, TIME.DeltaTime());

		inputManager.ProcessInput();

		camera.GetController()->SetDeltaTime(TIME.DeltaTime());
		camera.SetSpeed(cameraSpeed);

		renderer.Draw(activeScene, camera, TIME.DeltaTime());


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
