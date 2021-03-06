#pragma once

//FPS контроллер
#include "Camera/CameraController.h"

class Camera;
class FPSCameraController : public CameraController {
private:
	Camera* m_Camera = nullptr;

	float m_speedMul = 2;
	float m_speedRun = m_speedMul;

	bool m_isCursorLocked = true;

	float m_yaw = -90, m_pitch = 0;
	int m_lastX = 475, m_lastY = 270;

public:
	FPSCameraController();

	void BindCamera(Camera* camera) override;

	void InputProcessor(GLFWwindow* window) override;
	void MouseProcessor(GLFWwindow* window) override;
};

