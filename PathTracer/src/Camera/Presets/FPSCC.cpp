#include "FPSCC.h"
#include "Camera/Camera.h"

FPSCameraController::FPSCameraController()
{
}

void FPSCameraController::BindCamera(Camera* camera)
{
	m_Camera = camera;
}

void FPSCameraController::InputProcessor(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetSpeed() *
            CameraController::m_DeltaTime * m_Camera->GetDirection());
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetSpeed() *
            CameraController::m_DeltaTime * m_Camera->GetDirection());

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera->SetPosition(
            m_Camera->GetPosition() + 
            glm::normalize(glm::cross(m_Camera->GetDirection(), m_Camera->CalculateUp())) *
            CameraController::m_DeltaTime *
            m_Camera->GetSpeed()
        );

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera->SetPosition(m_Camera->GetPosition() -
            glm::normalize(glm::cross(m_Camera->GetDirection(), m_Camera->CalculateUp())) *
            CameraController::m_DeltaTime *
            m_Camera->GetSpeed()
        );
}
