#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CameraController.h"
#include "Presets/FPSCC.h"

enum class CameraType {
	CAM_FPS,
	CAM_ORBITAL,
};

class CameraController;
class Camera {
private:
	glm::mat4 m_View;
	CameraType m_Type;
	glm::vec3 m_CamPos;
	glm::vec3 m_CamDir;
	float m_FOV;
	float m_CamSpeed;

	CameraController* m_Controller;

	void InitFPSCamera();
public:
	Camera();
	Camera(CameraType type, glm::vec3 camPos, glm::vec3 camTarget, float fov, float camSpeed = 5.5f);

	CameraController* GetController();
	//будет обращаться к камера контроллеру и помещать его в вектор контроллеров на обработку
	glm::mat4 GetViewMatrix();

	glm::vec3 CalculateUp();
	inline void		SetFov(float fov) { m_FOV = fov; }
	inline float	GetFov() { return m_FOV; }

	inline void		SetSpeed(float newSpeed) { m_CamSpeed = newSpeed; }
	inline float	GetSpeed() { return m_CamSpeed; }

	inline glm::vec3 GetPosition() { return m_CamPos; }
	inline void		 SetPosition(glm::vec3 newPos) { m_CamPos = newPos; }

	inline glm::vec3 GetDirection() { return m_CamDir; }
	inline void		 SetDirection(glm::vec3 newDir) { m_CamDir = newDir; }

	inline CameraType GetType() { return m_Type; }
	inline void		 SetType(CameraType newType) { m_Type = newType; }
};
