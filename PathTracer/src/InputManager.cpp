#include "InputManager.h"


InputManager::InputManager(GLFWwindow* window)
	: m_Window(window)
{
}

void InputManager::ProcessInput()
{
	for (auto& controller : m_Controllers)
	{
		controller->InputProcessor(m_Window);
	}
}

void InputManager::Push(InputController* controller)
{
	m_Controllers.emplace_back(controller);
}
