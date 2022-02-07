#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera/Camera.h"

#include "Presets/FPSCC.h"

class CameraFPS : public Camera {
private:
	glm::mat4 m_View;
	glm::vec3 m_CamPos;
	glm::vec3 m_CamDir;
	float m_CamFov;
	float m_CamSpeed;
	glm::vec3 m_CamUpVec;

	CameraController* m_Controller;

    void InitCamera();
public:
	CameraFPS();
	CameraFPS(glm::vec3 camPos, glm::vec3 camTarget, float fov, float camSpeed = 5.5f);
	~CameraFPS();

	void SetCameraView(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 camUp) override;

	//будет обращаться к камера контроллеру и помещать его в вектор контроллеров на обработку
	CameraController* GetController() override;
	glm::mat4 GetViewMatrix() override;

	glm::vec3 UpdateUp() override;

	glm::vec3 GetPosition() override;
	void SetPosition(glm::vec3 newPos) override;
	glm::vec3 GetDirection() override;
	void SetDirection(glm::vec3 newDir) override;
	float GetSpeed() override;
	void SetSpeed(float newSpeed) override;
	void SetFov(float fov) override;
	float GetFov() override;
};
