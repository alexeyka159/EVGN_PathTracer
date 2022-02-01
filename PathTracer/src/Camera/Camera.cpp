#include "Camera.h"
#include "CameraController.h"



Camera::Camera()
	: m_Type(CameraType::CAM_ORBITAL),
	  m_CamPos(glm::vec3(5.0f, 0.0f, 0.0f)),
	  m_CamDir(glm::vec3(-1.0f, 0.0f, 0.0f)),
	  m_FOV(45.0f)
{
	m_Controller = new FPSCameraController();
	m_Controller->BindCamera(this);
}

Camera::Camera(CameraType type, glm::vec3 camPos, glm::vec3 camTarget, float fov, float camSpeed)
	: m_Type(type),
	  m_CamPos(camPos),
	  m_CamDir(glm::normalize(m_CamPos - camTarget)),
	  m_FOV(fov),
	  m_CamSpeed(camSpeed)
{
	
	//Не уверен, что это работает
	

	switch (type)
	{
	case CameraType::CAM_FPS:
		m_Controller = new FPSCameraController();
		InitFPSCamera();
		break;
	case CameraType::CAM_ORBITAL:
		break;
	}
	m_Controller->BindCamera(this);
}

void Camera::InitFPSCamera()
{
	m_View = glm::lookAt(m_CamPos, m_CamPos + glm::vec3(0.0f, 0.0f, -1.0f), CalculateUp());
}

CameraController* Camera::GetController()
{
	return m_Controller;
}

glm::mat4 Camera::GetViewMatrix()
{
	m_View = glm::lookAt(m_CamPos, m_CamPos + glm::vec3(0.0f, 0.0f, -1.0f), CalculateUp());
	return m_View;
}

glm::vec3 Camera::CalculateUp() {
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, m_CamDir));
	return glm::cross(m_CamDir, cameraRight);
}
