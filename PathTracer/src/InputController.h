#pragma once

#include <GLFW/glfw3.h>

//��������� ��������� �����
class InputController {
public:
	virtual void InputProcessor(GLFWwindow* window) = 0;
	virtual void MouseProcessor(GLFWwindow* window) = 0;
};