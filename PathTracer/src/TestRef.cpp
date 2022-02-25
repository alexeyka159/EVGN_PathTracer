#include "Evergreen.h"

int main()
{
	Renderer renderer(800, 600, "Evergreen Engene");

	Scene scene("res/scene.evgn");

	EVGN::GUI gui();

	renderer.SetCamera(camera);
	renderer.SetGui(gui);

	InputManager inputManager(renderer.GetWindow());
	inputManager.Push(renderer.GetCamera()->GetController());
	inputManager.Push(renderer.GetGui()->GetController());

	while (renderer.Running())
	{
		inputManager.ProcessInput();
		renderer.GetGui->Draw();
		renderer.Render(&scene);

		inputManager.PollEvents();
	}

	return 0;
}