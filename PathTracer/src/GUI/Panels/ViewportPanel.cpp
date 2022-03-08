#include "ViewportPanel.h"

#include <iostream>

ViewportPanel::ViewportPanel(Framebuffer& framebuffer, Camera& camera)
    : m_Framebuffer(&framebuffer)
    , m_GuiTextureID(0)
    , m_Camera(&camera)
{
}

void ViewportPanel::Draw()
{
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
