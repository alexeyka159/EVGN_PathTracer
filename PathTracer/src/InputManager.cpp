#include "InputManager.h"
#include "GLFW/glfw3.h"

InputManager::InputManager(GLFWwindow* window)
	: m_Window(window)
{
}

void InputManager::ProcessInput()
{

	for (auto& controller : m_Controllers)
	{
		controller->InputProcessor(m_Window);
		controller->MouseProcessor(m_Window);
	}
}

void InputManager::Push(InputController* controller)
{
	m_Controllers.emplace_back(controller);
}

void InputManager::PollEvents()
{
	glfwPollEvents();
}
