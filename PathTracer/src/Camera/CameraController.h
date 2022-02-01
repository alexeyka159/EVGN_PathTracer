#pragma once

//Контроллер камеры

#include "Camera/Camera.h"
#include "InputController.h"

class Camera;
class CameraController : public InputController {
private:
	Camera* m_Camera = nullptr;

public:
	CameraController();
	//CameraController(Camera* camera);

	void BindCamera(Camera* camera);

	void InputProcessor(GLFWwindow* window) override;
};

