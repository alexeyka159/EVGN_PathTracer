#pragma once

#include "GUIPanel.h"
#include "Framebuffer.h"
#include "Camera/CameraOrbit.h"

class ViewportPanel : public GUIPanel
{
private:
    uint32_t m_GuiTextureID;
    Framebuffer* m_Framebuffer;
    Camera* m_Camera = nullptr;
    glm::vec2 m_ViewportSize;
public:
    ViewportPanel(Framebuffer& framebuffer, Camera& camera);
    inline ~ViewportPanel() { delete this; };

    void Draw() override;
};