#include "CameraFPS.h"

CameraFPS::CameraFPS()
	: m_CamPos(glm::vec3(5.0f, 0.0f, 0.0f)),
	  m_CamDir(glm::vec3(-1.0f, 0.0f, 0.0f)),
	  m_CamSpeed(5.f),
	  m_CamFov(45.0f),
	  m_View(glm::mat4(1))
{
	InitCamera();
	m_Controller = new FPSCameraController();
	m_Controller->BindCamera(this);
}

CameraFPS::CameraFPS(glm::vec3 camPos, glm::vec3 camTarget, float fov, float camSpeed)
	: m_CamPos(camPos),
	  m_CamDir(camTarget),
	  m_CamFov(fov),
	  m_CamSpeed(camSpeed),
	  m_View(glm::mat4(1))
{
	InitCamera();
	m_Controller = new FPSCameraController();
	m_Controller->BindCamera(this);
}

CameraFPS::~CameraFPS()
{
	delete m_Controller;
}

void CameraFPS::SetCameraView(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 camUp)
{
	m_CamDir = camTarget;
	m_CamPos = camPos;
	m_CamUpVec = camUp;
	GetViewMatrix();
}

void CameraFPS::InitCamera()
{
	m_CamDir = glm::normalize(m_CamPos - m_CamDir);
	m_View = glm::lookAt(m_CamPos, m_CamPos + m_CamDir, UpdateUp());
}

CameraController* CameraFPS::GetController()
{
	return m_Controller;
}

glm::mat4 CameraFPS::GetViewMatrix()
{
	m_View = glm::lookAt(m_CamPos, m_CamPos + m_CamDir, UpdateUp());
	return m_View;
}

glm::vec3 CameraFPS::UpdateUp() {
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, m_CamDir));
	m_CamUpVec = glm::cross(m_CamDir, cameraRight);
	return m_CamUpVec;
}

glm::vec3 CameraFPS::GetPosition() { return m_CamPos; }
void CameraFPS::SetPosition(glm::vec3 newPos) {	m_CamPos = newPos; }

glm::vec3 CameraFPS::GetDirection() { return m_CamDir; }
void CameraFPS::SetDirection(glm::vec3 newDir) { m_CamDir = newDir; }

float CameraFPS::GetSpeed() { return m_CamSpeed; }
void CameraFPS::SetSpeed(float newSpeed) { m_CamSpeed = newSpeed; }

void CameraFPS::SetFov(float fov) {	m_CamFov = fov; }
float CameraFPS::GetFov() {	return m_CamFov; }
