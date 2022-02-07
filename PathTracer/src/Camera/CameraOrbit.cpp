#include "Camera/CameraOrbit.h"

void CameraOrbit::InitCamera()
{
}

CameraOrbit::CameraOrbit()
	: m_CamPos(glm::vec3(5.0f, 0.0f, 0.0f)),
	  m_CamDir(glm::vec3(0.0f, 0.0f, 0.0f)),
	  m_CamUpVec(glm::vec3(0.f, 1.f, 0.f)),
	  m_CamSpeed(5.f),
	  m_CamFov(45.0f),
	  m_View(glm::mat4(1))
{
	GetViewMatrix();
	m_Controller = new OrbitCameraController();
	m_Controller->BindCamera(this);
}

CameraOrbit::CameraOrbit(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 upVec, float fov, float camSpeed)
	: m_CamPos(camPos),
	  m_CamDir(camTarget),
	  m_CamFov(fov),
	  m_CamUpVec(upVec),
	  m_CamSpeed(camSpeed),
	  m_View(glm::mat4(1))
{
	GetViewMatrix();
	m_Controller = new OrbitCameraController();
	m_Controller->BindCamera(this);
}

CameraOrbit::~CameraOrbit()
{
	delete m_Controller;
}

void CameraOrbit::SetCameraView(glm::vec3 camPos, glm::vec3 camTarget, glm::vec3 camUp)
{
	m_CamDir = camTarget;
	m_CamPos = camPos;
	m_CamUpVec = camUp;

	GetViewMatrix();
}

glm::mat4 CameraOrbit::GetViewMatrix() {
	m_View = glm::lookAt(m_CamPos, m_CamDir, m_CamUpVec);
	return m_View;
}

CameraController* CameraOrbit::GetController() { return m_Controller; }

glm::vec3 CameraOrbit::UpdateUp()
{
	//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	//glm::vec3 cameraRight = glm::normalize(glm::cross(up, m_CamDir));
	//return glm::cross(m_CamDir, cameraRight);
	//std::cout << m_CamUpVec.x << " " << m_CamUpVec.y << " " << m_CamUpVec.z << std::endl;
	return m_CamUpVec;
}

glm::vec3 CameraOrbit::GetPosition() { return m_CamPos; }
void CameraOrbit::SetPosition(glm::vec3 newPos) { m_CamPos = newPos; }

glm::vec3 CameraOrbit::GetDirection() { return m_CamDir; }
glm::vec3 CameraOrbit::GetRightVector() { return glm::transpose(m_View)[0]; }

void CameraOrbit::SetDirection(glm::vec3 newDir) { m_CamDir = newDir; }

float CameraOrbit::GetSpeed() { return m_CamSpeed; }
void CameraOrbit::SetSpeed(float newSpeed) { m_CamSpeed = newSpeed; }

void CameraOrbit::SetFov(float fov) { m_CamFov = fov; }
float CameraOrbit::GetFov() { return m_CamFov; }
