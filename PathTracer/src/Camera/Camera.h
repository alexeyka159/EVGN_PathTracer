#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraController;
class Camera {
private:
	glm::vec3 m_CamPos;
	glm::vec3 m_CamDir;
	float m_CamFov;
	float m_CamSpeed;

	//float m_CamUpVec;
public:

	virtual void SetCameraView(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 camUp) = 0;

	virtual glm::mat4 GetViewMatrix() = 0;

	virtual glm::mat4 GetProjection() = 0;
	virtual void SetProjection(float& width, float& height, float& fov, float& near, float& far) = 0;
	virtual void OnResize(float& width, float& height) = 0;

	virtual glm::vec3 UpdateUp() = 0;
	virtual glm::vec3 GetRightVector() = 0;

	virtual CameraController* GetController() = 0;
	
	virtual glm::vec3 GetPosition() = 0;
	virtual void SetPosition(glm::vec3 newPos) = 0;

	virtual glm::vec3 GetDirection() = 0;
	virtual void SetDirection(glm::vec3 newDir) = 0;

	virtual float GetSpeed() = 0;
	virtual void SetSpeed(float newSpeed) = 0;

	virtual void SetFov(float fov) = 0;
	virtual float GetFov() = 0;

	virtual void SetNear(float near) = 0;
	virtual float GetNear() = 0;

	virtual void SetFar(float far) = 0;
	virtual float GetFar() = 0;
};
