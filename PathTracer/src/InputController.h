#pragma once

#include <GLFW/glfw3.h>

//Интерфейс обработки ввода
class InputController {
public:
	virtual void InputProcessor(GLFWwindow* window) = 0;
};