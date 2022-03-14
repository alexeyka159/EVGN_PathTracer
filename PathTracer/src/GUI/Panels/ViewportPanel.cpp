#include "ViewportPanel.h"

#include <iostream>



ViewportPanel::ViewportPanel(Framebuffer& framebuffer, Camera& camera/*, Scene& context, Model& defaultModel, Shader& trailShader, GLFWwindow* window*/)
    : m_Framebuffer(&framebuffer)
    , m_GuiTextureID(0)
    , m_Camera(&camera)
    /*, m_Window(window)
    , m_Context(&context)
    , m_DefaultModel(&defaultModel)
    , m_TrailShader(&trailShader)*/
{
}

//void ViewportPanel::CreateBody()
//{
//    glm::vec3 viewDir = m_Camera->GetDirection() - m_Camera->GetPosition();
//
//    glm::mat4 iV = glm::inverse(m_Camera->GetViewMatrix());
//    glm::vec3 XAxis = iV * glm::vec4(1, 0, 0, 0);
//    glm::vec3 ZAxis = iV * glm::vec4(0, 0, 1, 0);
//
//    Entity entity = m_Context->CreateEntity();
//    entity.GetComponent<TransformComponent>().Translation += glm::vec3(XAxis.x, 0, ZAxis.z);
//    entity.AddComponent<ModelRendererComponent>(*m_DefaultModel);
//    entity.AddComponent<GravityComponent>(10.f, 1, glm::vec3(0, 0.0f, -1.85f));
//    entity.GetComponent<TransformComponent>().Scale = glm::vec3(entity.GetComponent<GravityComponent>().Radius/2);
//    entity.AddComponent<TrailComponent>(&m_TrailShader);
//}

void ViewportPanel::Draw()
{
   /* int mouseState = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT);
    if (mouseState == GLFW_PRESS && m_MouseState == false) {
        m_MouseState = true;
        glfwGetCursorPos(m_Window, &m_StartPosX, &m_StartPosY);
    }
    if (mouseState == GLFW_RELEASE && m_MouseState == true) {
        m_MouseState = false;
        glfwGetCursorPos(m_Window, &m_EndPosX, &m_EndPosY);
        CreateBody();
    }*/

    m_GuiTextureID = m_Framebuffer->GetColorAttachmentRendererId();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");
    ImVec2 viewportPanelSize = ImGui::GetContentRegionMax();
    
    if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
    {
        m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    }
    ImGui::Image((void*)m_GuiTextureID, ImVec2{ viewportPanelSize.x, viewportPanelSize.y -19 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    m_Camera->OnResize(viewportPanelSize.x, viewportPanelSize.y);
    ImGui::End();
    ImGui::PopStyleVar();
}
