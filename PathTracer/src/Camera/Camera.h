#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CameraController.h"

class CameraController;
class Camera {
private:
	glm::mat4 m_View;
	std::string m_Type;
	glm::vec3 m_CamPos;
	glm::vec3 m_CamDir;
	float m_FOV;

	CameraController* m_Controller;

public:
	Camera();
	Camera(std::string type, glm::vec3 camPos, glm::vec3 camDir, float fov);

	inline void SetFov(float fov) { m_FOV = fov; }
	inline float GetFov() { return m_FOV; }

	CameraController* GetController();
	//будет обращаться к камера контроллеру и помещать его в вектор контроллеров на обработку
	glm::mat4 GetViewMatrix();
};
