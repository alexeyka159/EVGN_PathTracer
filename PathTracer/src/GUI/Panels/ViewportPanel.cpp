#include "ViewportPanel.h"

#include <iostream>

ViewportPanel::ViewportPanel(Framebuffer& framebuffer, GLFWwindow& window, SceneHierarchyPanel& sceneHierarchyPanel, Camera& camera)
    : m_Framebuffer(&framebuffer)
    , m_GuiTextureID(0)
    , m_Camera(&camera)
    , m_Window(&window)
    , m_SceneHierarchyPanel(&sceneHierarchyPanel)
{
}

void ViewportPanel::Draw()
{
    m_Framebuffer->Bind();

    auto[mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
    my = viewportSize.y - my - 20;

    int mouseX = (int)mx;
    int mouseY = (int)my;

    if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
        mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
    {
        int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
        //std::cout << "Mouse = " << mouseX << ", " << mouseY << std::endl;
        //std::cout << "Pixel Data = " << pixelData << std::endl;
        m_SceneHierarchyPanel->SetSelection(pixelData);
    }


    m_GuiTextureID = m_Framebuffer->GetColorAttachmentRendererId();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");
    auto viewportOffset = ImGui::GetCursorPos();

    ImVec2 viewportPanelSize = ImGui::GetContentRegionMax();
    
    if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
    {
        m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    }
    ImGui::Image((void*)m_GuiTextureID, ImVec2{ viewportPanelSize.x, viewportPanelSize.y -19 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    m_Camera->OnResize(viewportPanelSize.x, viewportPanelSize.y);
    
    auto windowSize = ImGui::GetWindowSize();
    ImVec2 minBound = ImGui::GetWindowPos();
    minBound.x += viewportOffset.x;
    minBound.y += viewportOffset.y;

    ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
    m_ViewportBounds[0] = { minBound.x, minBound.y };
    m_ViewportBounds[1] = { maxBound.x, maxBound.y };

    ImGui::End();
    ImGui::PopStyleVar();

    m_Framebuffer->Unbind();
}
