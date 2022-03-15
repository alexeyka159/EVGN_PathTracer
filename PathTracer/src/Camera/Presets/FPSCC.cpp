#include "FPSCC.h"
#include "Camera/CameraFPS.h"

#include "GLFW/glfw3.h"

FPSCameraController::FPSCameraController() { }

void FPSCameraController::BindCamera(Camera* camera)
{
	m_Camera = camera;
}

void FPSCameraController::InputProcessor(GLFWwindow* window)
{
    //!!!При движении вбок, вектора складываются, и камера движется слишком быстро!!!
    //! -> Сначала считать все перемещение, в конце нормализировать этот вектор и передавать в позицию камеры
 
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        m_speedRun = m_speedMul;
    else
        m_speedRun = 1;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetSpeed() * m_speedRun *
            CameraController::m_DeltaTime * m_Camera->GetDirection());

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera->SetPosition(m_Camera->GetPosition() - m_Camera->GetSpeed() * m_speedRun *
            CameraController::m_DeltaTime * m_Camera->GetDirection());
    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera->SetPosition(m_Camera->GetPosition() -
            glm::normalize(glm::cross(m_Camera->GetDirection(), m_Camera->UpdateUp())) * m_speedRun *
            CameraController::m_DeltaTime *
            m_Camera->GetSpeed()
        );

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera->SetPosition(
            m_Camera->GetPosition() + 
            glm::normalize(glm::cross(m_Camera->GetDirection(), m_Camera->UpdateUp())) * m_speedRun *
            CameraController::m_DeltaTime *
            m_Camera->GetSpeed()
        );

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetSpeed() * m_speedRun *
            CameraController::m_DeltaTime * glm::vec3(0, 1.f, 0));

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_Camera->SetPosition(m_Camera->GetPosition() + m_Camera->GetSpeed() * m_speedRun *
            CameraController::m_DeltaTime * glm::vec3(0, -1.f, 0));

    /*if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if(m_isCursorLocked)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_isCursorLocked = !m_isCursorLocked;
    }

    */
   
    auto mouseModeCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            int windowWidth, windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
        }
        else if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    };

    glfwSetKeyCallback(window, mouseModeCallback);
}

void FPSCameraController::MouseProcessor(GLFWwindow* window)
{
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        
        float xoffset = xPos - m_lastX;
        float yoffset = m_lastY - yPos;
        m_lastX = xPos;
        m_lastY = yPos;

        float sensitivity = 0.05;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        direction.y = sin(glm::radians(m_pitch));
        direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_Camera->SetDirection(glm::normalize(direction));
}
