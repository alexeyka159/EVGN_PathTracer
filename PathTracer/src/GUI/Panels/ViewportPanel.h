#pragma once

#include "GUIPanel.h"
#include "Framebuffer.h"
#include "Camera/CameraOrbit.h"

#include "ViewportController.h"

class ViewportPanel : public GUIPanel
{
private:
    uint32_t m_GuiTextureID;
    Framebuffer* m_Framebuffer;
    Camera* m_Camera = nullptr;
    glm::vec2 m_ViewportSize;
    GLFWwindow* m_Window;
    ViewportController* m_Controller;

    bool m_MouseState = false;

public:
    ViewportPanel(Framebuffer& framebuffer, Camera& camera, ViewportController& controller);
    inline ~ViewportPanel() { delete this; };

    inline InputController* GetController() { return m_Controller; };

    void Draw() override;
};