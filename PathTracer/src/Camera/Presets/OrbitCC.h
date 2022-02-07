#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera/CameraController.h"

class Camera;
class OrbitCameraController : public CameraController {
private:
	Camera* m_Camera = nullptr;

	const double m_PI = 3.1415926;
	int m_lastX = 475, m_lastY = 270;
	double xPos = m_lastX, yPos = m_lastY;

	float m_SpeedMul = .07f;
	float m_Magnitude;

public:
	OrbitCameraController();

	void BindCamera(Camera* camera) override;

	void InputProcessor(GLFWwindow* window) override;
	void MouseProcessor(GLFWwindow* window) override;
};

