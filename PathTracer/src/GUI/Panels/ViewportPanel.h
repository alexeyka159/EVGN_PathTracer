#pragma once

#include "GUIPanel.h"
#include "Framebuffer.h"
#include "Camera/CameraOrbit.h"

//#include "Scene/Scene.h"
//#include "Model/Model.h"
//#include "Scene/Entity.h"

class ViewportPanel : public GUIPanel
{
private:
    uint32_t m_GuiTextureID;
    Framebuffer* m_Framebuffer;
    Camera* m_Camera = nullptr;
    glm::vec2 m_ViewportSize;
    GLFWwindow* m_Window;
    /*Scene* m_Context;
    Model* m_DefaultModel;
    Shader* m_TrailShader;*/

    double m_EndPosX = 475, m_EndPosY = 270;
    double m_StartPosX = m_EndPosX, m_StartPosY = m_EndPosY;

    bool m_MouseState = false;

public:
    ViewportPanel(Framebuffer& framebuffer, Camera& camera/*, Scene& context, Model& defaultModel, Shader& trailShader, GLFWwindow* window*/);
    inline ~ViewportPanel() { delete this; };

    //void CreateBody();

    void Draw() override;
};