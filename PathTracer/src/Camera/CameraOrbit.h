#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera/Camera.h"

#include "Presets/OrbitCC.h"

class CameraOrbit : public Camera {
private:
	glm::mat4 m_View;
	glm::mat4 m_Projection;

	glm::vec3 m_CamPos;
	glm::vec3 m_CamDir;
	float m_CamFov;
	float m_CamSpeed;
	glm::vec3 m_CamUpVec;
	float m_Near = 0.1f, m_Far = 100.f;

	CameraController* m_Controller;

	void InitCamera();
public:
	CameraOrbit();
	CameraOrbit(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 upVec, float fov, float camSpeed = 5.5f);
	CameraOrbit(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 upVec, float fov, float near, float far, float camSpeed = 5.5f);
	~CameraOrbit();

	void SetCameraView(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 camUp) override;

	//будет обращаться к камера контроллеру и помещать его в вектор контроллеров на обработку
	CameraController* GetController() override;
	glm::mat4 GetViewMatrix() override;

	glm::vec3 UpdateUp() override;

	inline glm::mat4 GetProjection() override { return m_Projection; };
	inline void SetProjection(float& width, float& height, float& fov, float& near, float& far) override {
		m_Projection = glm::perspective(glm::radians(fov), (float)width / (float)height, near, far);
	};

	void OnResize(float& width, float& height) override;

	glm::vec3 GetDirection() override;
	glm::vec3 GetRightVector() override;

	glm::vec3 GetPosition() override;
	void SetPosition(glm::vec3 newPos) override;
	void SetDirection(glm::vec3 newDir) override;
	float GetSpeed() override;
	void SetSpeed(float newSpeed) override;
	void SetFov(float fov) override;
	float GetFov() override;

	void SetNear(float near) override;
	float GetNear() override;

	void SetFar(float far) override;
	float GetFar() override;
};
