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
#include "GUI/Panels/ViewportController.h"

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

	int WIDTH = 1280, HEIGHT = 720;
	Renderer renderer(WIDTH, HEIGHT, "Gravity Simulation");
	
	EVGN::Time TIME;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SOURCE0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	float cameraSpeed = 5;
	glm::vec3 cameraPos = glm::vec3(0.0f, 40.0f, 45.0f);
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

		Entity skybox = activeScene.CreateEntity("Skybox");
		skybox.AddComponent<ModelRendererComponent>(Model("res/models/Skybox.obj", sunShader));
		skybox.GetComponent<TransformComponent>().Scale = glm::vec3(100);

		Entity sun = activeScene.CreateEntity("Sun");
		sun.AddComponent<ModelRendererComponent>(Model("res/models/Sun.obj", sunShader));
		sun.GetComponent<TransformComponent>().Translation += glm::vec3(0, 1.5f, 0);
		sun.AddComponent<GravityComponent>(100, 5, glm::vec3(0.0f, 0.0f, 0.0f));
		sun.GetComponent<TransformComponent>().Scale = glm::vec3(sun.GetComponent<GravityComponent>().Radius / 2);
		sun.AddComponent<TrailComponent>(&trailShader);

		Entity mercury = activeScene.CreateEntity("Mercury");
		mercury.AddComponent<ModelRendererComponent>("res/models/Mercury.obj", asteroidShader);
		mercury.GetComponent<TransformComponent>().Translation += glm::vec3(18, 0.0f, 0.0f);
		mercury.AddComponent<GravityComponent>(.1f, .2f, glm::vec3(0, 0.0f, -2.22f));
		mercury.GetComponent<TransformComponent>().Scale = glm::vec3(mercury.GetComponent<GravityComponent>().Radius / 2);
		mercury.AddComponent<TrailComponent>(&trailShader);

		Entity venus = activeScene.CreateEntity("Venus");
		venus.AddComponent<ModelRendererComponent>("res/models/Venus.obj", asteroidShader);
		venus.GetComponent<TransformComponent>().Translation += glm::vec3(22, -1.2f, 0.0f);
		venus.AddComponent<GravityComponent>(.22f, .3f, glm::vec3(0, 0.0f, -1.9f));
		venus.GetComponent<TransformComponent>().Scale = glm::vec3(venus.GetComponent<GravityComponent>().Radius / 2);
		venus.AddComponent<TrailComponent>(&trailShader);

		Entity earth = activeScene.CreateEntity("Earth");
		earth.AddComponent<ModelRendererComponent>("res/models/earth.obj", asteroidShader);
		earth.GetComponent<TransformComponent>().Translation += glm::vec3(35.f, 1.2f, 0.0f);
		earth.AddComponent<GravityComponent>(1.2f, .9, glm::vec3(0, 0.0f, -1.6f));
		earth.GetComponent<TransformComponent>().Scale = glm::vec3(earth.GetComponent<GravityComponent>().Radius / 2);
		earth.AddComponent<TrailComponent>(&trailShader);

		Entity moon = activeScene.CreateEntity("Moon");
		moon.AddComponent<ModelRendererComponent>(asteroidModel);
		moon.GetComponent<TransformComponent>().Translation += glm::vec3(37.5f, 1.2f, 0.0f);
		moon.AddComponent<GravityComponent>(.1f, .2f, glm::vec3(0, 0.0f, -1.f));
		moon.GetComponent<TransformComponent>().Scale = glm::vec3(moon.GetComponent<GravityComponent>().Radius / 2);
		moon.AddComponent<TrailComponent>(&trailShader);


		Entity mars = activeScene.CreateEntity("Mars");
		mars.AddComponent<ModelRendererComponent>("res/models/Mars.obj", asteroidShader);
		mars.GetComponent<TransformComponent>().Translation += glm::vec3(50.f, 0.f, 0.0f);
		mars.AddComponent<GravityComponent>(.6f, .8, glm::vec3(0, 0.0f, -1.3f));
		mars.GetComponent<TransformComponent>().Scale = glm::vec3(mars.GetComponent<GravityComponent>().Radius / 2);
		mars.AddComponent<TrailComponent>(&trailShader);

		Entity jupiter = activeScene.CreateEntity("Jupiter");
		jupiter.AddComponent<ModelRendererComponent>("res/models/Jupiter.obj", asteroidShader);
		jupiter.GetComponent<TransformComponent>().Translation += glm::vec3(70.f, 0.f, 0.0f);
		jupiter.AddComponent<GravityComponent>(2.f, 2.2f, glm::vec3(0, 0.0f, -1.1f));
		jupiter.GetComponent<TransformComponent>().Scale = glm::vec3(jupiter.GetComponent<GravityComponent>().Radius / 2);
		jupiter.AddComponent<TrailComponent>(&trailShader);

		Entity saturn = activeScene.CreateEntity("Saturn");
		saturn.AddComponent<ModelRendererComponent>("res/models/Saturn.obj", asteroidShader);
		saturn.GetComponent<TransformComponent>().Translation += glm::vec3(-90.f, 0.f, 0.0f);
		saturn.AddComponent<GravityComponent>(1.2f, 2.f, glm::vec3(0, 0.0f, 1.f));
		saturn.GetComponent<TransformComponent>().Scale = glm::vec3(saturn.GetComponent<GravityComponent>().Radius / 2);
		saturn.AddComponent<TrailComponent>(&trailShader);

		Entity uranus = activeScene.CreateEntity("Uranus");
		uranus.AddComponent<ModelRendererComponent>("res/models/Uranus.obj", asteroidShader);
		uranus.GetComponent<TransformComponent>().Translation += glm::vec3(95.f, 0.f, 0.0f);
		uranus.AddComponent<GravityComponent>(1.3f, 1.1f, glm::vec3(0, 0.0f, -0.7f));
		uranus.GetComponent<TransformComponent>().Scale = glm::vec3(uranus.GetComponent<GravityComponent>().Radius / 2);
		uranus.AddComponent<TrailComponent>(&trailShader);

		Entity neptune = activeScene.CreateEntity("Neptune");
		neptune.AddComponent<ModelRendererComponent>("res/models/Neptune.obj", asteroidShader);
		neptune.GetComponent<TransformComponent>().Translation += glm::vec3(115.f, 3.f, 0.0f);
		neptune.AddComponent<GravityComponent>(1.3f, 1.1f, glm::vec3(0, 0.0f, -1.f));
		neptune.GetComponent<TransformComponent>().Scale = glm::vec3(neptune.GetComponent<GravityComponent>().Radius / 2);
		neptune.AddComponent<TrailComponent>(&trailShader);

	Gravity gravity;

	glm::vec3 lightPos(0.0f, 3.0f, 0.f);

	FramebufferSpecification fbSpec;
	fbSpec.Width = WIDTH;
	fbSpec.Height = HEIGHT;
	Framebuffer frameBuffer(fbSpec);


	GUI gui(renderer.GetWindow());

	ViewportController vpController(activeScene, camera, asteroidModel, trailShader);
	ViewportPanel* viewport = new ViewportPanel(frameBuffer, camera, vpController);
	gui.Push(viewport);
	inputManager.Push(viewport->GetController());

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
		gui.Render();
		gui.End();

		inputManager.PollEvents();
		glfwSwapBuffers(renderer.GetWindow());
	}

	return 0;
}
