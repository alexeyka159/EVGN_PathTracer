#pragma once

struct GLFWwindow;
//��������� ��������� �����
class InputController {
public:
	virtual void InputProcessor(GLFWwindow* window) = 0;
	virtual void MouseProcessor(GLFWwindow* window) = 0;
};