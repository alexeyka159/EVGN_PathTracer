#pragma once

#include "GUIPanel.h"
#include "Framebuffer.h"
#include "Camera/CameraOrbit.h"
#include "SceneHierarchyPanel.h"
#include <ImGuizmo.h>

class ViewportPanel : public GUIPanel
{
private:
    uint32_t m_GuiTextureID;
    Framebuffer* m_Framebuffer;
    GLFWwindow* m_Window;
    Camera* m_Camera = nullptr;
    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
    glm::vec2 m_ViewportBounds[2];
    SceneHierarchyPanel* m_SceneHierarchyPanel;

    int m_GizmoType = -1;

public:
    ViewportPanel(Framebuffer& framebuffer, GLFWwindow& window, SceneHierarchyPanel& sceneHierarchyPanel, Camera& camera);
    inline ~ViewportPanel() { delete this; };

    void Draw() override;
};