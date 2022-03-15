#include "ViewportController.h"



ViewportController::ViewportController(Scene& context, Camera& camera, Model& defaultModel, Shader& trailShader)
    : m_Context(&context)
    , m_Camera(&camera)
    , m_DefaultModel(&defaultModel)
    , m_TrailShader(&trailShader)
{
}

void ViewportController::InputProcessor(GLFWwindow* window)
{
}

void ViewportController::CreateBody()
{
    glm::vec3 velocity = (m_SpawnPos - m_ReleasePos) / 3.f;
    
    Entity entity = m_Context->CreateEntity("Asteroid");
    entity.GetComponent<TransformComponent>().Translation = m_SpawnPos;
    entity.AddComponent<ModelRendererComponent>(*m_DefaultModel);
    entity.AddComponent<GravityComponent>(5.f, .5f, velocity);
    entity.GetComponent<TransformComponent>().Scale = glm::vec3(entity.GetComponent<GravityComponent>().Radius/2);
    entity.AddComponent<TrailComponent>(m_TrailShader);
}

glm::vec3 ViewportController::ScreenToWorldSpace(double& x, double& y)
{
    glm::vec3 viewDir = m_Camera->GetDirection() - m_Camera->GetPosition();

    glm::vec4 cursorNormalPos(2 * x / m_ScreenSize.x - 1, -2 * (y - 25) / m_ScreenSize.y + 1, -1, 1.0f);
    glm::vec4 inverseCursorPos(glm::vec2(glm::inverse(m_Camera->GetProjection()) * cursorNormalPos), -1, 0);
    glm::vec3 direction(glm::vec3(glm::inverse(m_Camera->GetViewMatrix()) * inverseCursorPos));
    direction = glm::normalize(direction);
    glm::vec3 camPos = glm::vec3(glm::inverse(m_Camera->GetViewMatrix()) * glm::vec4(0, 0, 0, 1.0f));

    glm::vec3 N(0.0f, 1.0f, 0.0f);
    float t = -glm::dot(camPos, N) / glm::dot(direction, N);
    return (camPos + direction * t);
}

void ViewportController::MouseProcessor(GLFWwindow* window)
{
    int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (mouseState == GLFW_PRESS && m_MouseState == false && m_Editable) {
        m_MouseState = true;
        glfwGetCursorPos(window, &m_StartPosX, &m_StartPosY);
        m_SpawnPos = ScreenToWorldSpace(m_StartPosX, m_StartPosY);
    }
    if (mouseState == GLFW_RELEASE && m_MouseState == true) {
        m_MouseState = false;
        glfwGetCursorPos(window, &m_EndPosX, &m_EndPosY);
        m_ReleasePos = ScreenToWorldSpace(m_EndPosX, m_EndPosY);
        CreateBody();
    }
}
