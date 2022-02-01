#pragma once

//Контроллер камеры

//#include "Camera/Camera.h"
#include "InputController.h"

class Camera;
class CameraController : public InputController {
private:
	//Camera* m_Camera = nullptr;

public:
	float m_DeltaTime;

	//CameraController();
	//CameraController(Camera* camera);

	virtual void BindCamera(Camera* camera) = 0;

	virtual void InputProcessor(GLFWwindow* window) override = 0;

	inline void SetDeltaTime(float dt) { m_DeltaTime = dt; }
};

