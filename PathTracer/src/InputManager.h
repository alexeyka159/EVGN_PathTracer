#pragma once

//Обработчик событий

//#include <functional>
#include <vector>
#include "InputController.h"

class InputManager {
private:
	GLFWwindow* m_Window;
	std::vector<InputController*> m_Controllers;

public:
	InputManager(GLFWwindow* window);
	
	void ProcessInput();
	void Push(InputController* controller);
	void PollEvents();
};