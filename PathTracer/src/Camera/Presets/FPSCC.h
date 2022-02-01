#pragma once

//FPS контроллер

//#include "Camera/Camera.h"
#include "Camera/CameraController.h"

class Camera;
class FPSCameraController : public CameraController {
private:
	Camera* m_Camera = nullptr;

public:
	FPSCameraController();
	//FPSCameraController(Camera* camera);

	void BindCamera(Camera* camera) override;

	void InputProcessor(GLFWwindow* window) override;
};

