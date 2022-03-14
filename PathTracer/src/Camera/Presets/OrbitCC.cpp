#include "OrbitCC.h"
#include "Camera/Camera.h"

#include <iostream>

OrbitCameraController::OrbitCameraController() { }

void OrbitCameraController::BindCamera(Camera* camera)
{
	m_Camera = camera;
}

void OrbitCameraController::InputProcessor(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		m_Camera->SetCameraView(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	if (mouseState == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &xPos, &yPos);

		float offsetSpeed = 0.05f;
		float mouseOffset = m_lastY - yPos;
		
		glm::vec3 viewDir = m_Camera->GetDirection() - m_Camera->GetPosition();
		m_Magnitude = glm::max(glm::tan(glm::min(glm::radians(glm::length(viewDir+.5f)), 0.4f)), 0.00001f);

		m_Camera->SetPosition(m_Camera->GetPosition() + glm::normalize(viewDir) * mouseOffset * offsetSpeed * m_Magnitude * m_Camera->GetSpeed());
	}

}

void OrbitCameraController::MouseProcessor(GLFWwindow* window)
{
	glm::vec3 viewDir = glm::normalize(m_Camera->GetDirection() - m_Camera->GetPosition());
	glm::vec3 upVector = glm::vec3(0, 1, 0);
	glm::vec3 rightVector = glm::cross(viewDir, upVector);

	// Get the homogenous position of the camera and pivot point
	glm::vec4 position(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z, 1);
	
	// step 1 : Calculate the amount of rotation given the mouse movement.
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	
	glfwGetCursorPos(window, &xPos, &yPos);

	float deltaAngleX = (2 * m_PI / windowWidth); //движение слева направо = 2*PI = 360 deg
	float deltaAngleY = (m_PI / windowHeight);  //движение сверху вниз = PI = 180 deg

	int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	if (mouseState == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		glm::vec4 pivot = glm::vec4(m_Camera->GetDirection().x, m_Camera->GetDirection().y, m_Camera->GetDirection().z, 1);

		//Беру максимум от нижней границы(0.00001) и тангенса магнитуды вектора точки вращения и позиции камеры в радианах
		m_Magnitude = glm::max(glm::tan(glm::radians(glm::length(viewDir+.5f))), 0.00001f);

		//Угол вращения на сфере = ((сдвиг курсора) * время между этим и предыдущим кадрами * чувствительность камеры *
		//							 скорость вращения камеры * размер сферы вращения * фикс разрещения окна) *
		//							(1/магнитуда расстояния до точки вращения)
		float xAngle = ((m_lastX - xPos) * m_DeltaTime * m_Camera->GetSpeed() * m_SpeedMul * deltaAngleX * 150)* ((1 / ((double)m_Magnitude))/4.f);
		float yAngle = ((m_lastY - yPos) * m_DeltaTime * m_Camera->GetSpeed() * m_SpeedMul * deltaAngleY * 170)* ((1 / ((double)m_Magnitude))/4.f);

		glm::mat4 iV = glm::inverse(m_Camera->GetViewMatrix());
		glm::vec3 horizontalAxis = iV * glm::vec4(1, 0, 0, 0);
		glm::vec3 verticalAxis = iV * glm::vec4(0, -1, 0, 0);

		glm::vec3 translateVec = horizontalAxis * xAngle + verticalAxis * yAngle;

		pivot = pivot + glm::vec4(translateVec, 1.f);
		position = position + glm::vec4(translateVec, 1.f);

		m_Camera->SetCameraView(position, pivot, m_Camera->UpdateUp());
	}
	else if (mouseState == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		glm::vec4 pivot = glm::vec4(m_Camera->GetDirection().x, m_Camera->GetDirection().y, m_Camera->GetDirection().z, 1);

		float xAngle = (m_lastX - xPos) * deltaAngleX * m_DeltaTime * m_Camera->GetSpeed() / m_SpeedMul;
		float yAngle = (m_lastY - yPos) * deltaAngleY * m_DeltaTime * m_Camera->GetSpeed() / m_SpeedMul;

		// Extra step to handle the problem when the camera direction is the same as the up vector
		float cosAngle = glm::dot(viewDir, upVector);
		if (cosAngle * glm::sign(yAngle) > 0.99f)
			yAngle = 0;

		// step 2: Rotate the camera around the pivot point on the first axis.
		glm::mat4x4 rotationMatrixX(1.0f);
		rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, upVector);
		position = (rotationMatrixX * (position - pivot)) + pivot;

		// step 3: Rotate the camera around the pivot point on the second axis.
		glm::mat4x4 rotationMatrixY(1.0f);
		rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, rightVector);
		glm::vec3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;

		// Update the camera view (we keep the same lookat and the same up vector)
		m_Camera->SetCameraView(finalPosition, pivot, m_Camera->UpdateUp());
	}

	// Update the mouse position for the next rotation
	m_lastX = xPos;
	m_lastY = yPos;
}
