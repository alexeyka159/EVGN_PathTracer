#include "CameraController.h"

//будет принимать из контроллера результаты ввода и возвращать их камере

CameraController::CameraController()
{
}

void CameraController::BindCamera(Camera* camera)
{
	m_Camera = camera;
}

void CameraController::InputProcessor(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		m_Camera->SetFov(m_Camera->GetFov() + 1);
}
