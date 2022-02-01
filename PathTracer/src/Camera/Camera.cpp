#include "Camera.h"
#include "CameraController.h"

Camera::Camera()
	: m_Type("PERSPECTIVE"),
	  m_CamPos(glm::vec3(5.0f, 0.0f, 0.0f)),
	  m_CamDir(glm::vec3(-1.0f, 0.0f, 0.0f)),
	  m_FOV(45.0f)
{
	m_Controller = new CameraController();
	m_Controller->BindCamera(this);
}

Camera::Camera(std::string type, glm::vec3 camPos, glm::vec3 camDir, float fov)
	: m_Type(type),
	  m_CamPos(camPos),
	  m_CamDir(camDir),
	  m_FOV(fov)
{
	m_Controller = new CameraController();
	//Не уверен, что это работает
	m_Controller->BindCamera(this);
}

CameraController* Camera::GetController()
{
	return m_Controller;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::mat4();
}
