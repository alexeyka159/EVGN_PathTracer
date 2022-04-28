#include "ViewportPanel.h"

#include <iostream>
#include "glm/gtc/type_ptr.hpp"

#include "Math/Math.h"

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


    //Выделение
    if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS &&
        mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
    {
        int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
        //std::cout << "Mouse = " << mouseX << ", " << mouseY << std::endl;
        //std::cout << "Pixel Data = " << pixelData << std::endl;
        m_SceneHierarchyPanel->SetSelection(pixelData);
    }

    //Гизмо хоткеи
    if (glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS)
        m_GizmoType = -1;
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
        m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
    if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS)
        m_GizmoType = ImGuizmo::OPERATION::SCALE;
    if (glfwGetKey(m_Window, GLFW_KEY_R) == GLFW_PRESS)
        m_GizmoType = ImGuizmo::OPERATION::ROTATE;
    bool snap;
    if (glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        snap = true;
    else
        snap = false;


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

    //Gizmos
    Entity selectedEntity = m_SceneHierarchyPanel->GetSelectedEntity();
    if (selectedEntity && m_GizmoType != -1)
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

        //TODO: Поиск по сцене в поисках активной камеры

        glm::mat4 cameraProjection = m_Camera->GetProjection();
        glm::mat4 cameraView = m_Camera->GetViewMatrix();

        auto& tc = selectedEntity.GetComponent<TransformComponent>();
        glm::mat4 transform = tc.GetTransform();

        float snapValue = 0.5f;
        if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 45.f;

        float snapValues[3] = { snapValue, snapValue, snapValue};

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
            (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
            nullptr, snap ? snapValues : nullptr);

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation, rotation, scale;
            Math::DecomposeTransform(transform, translation, rotation, scale);

            glm::vec3 deltaRotation = rotation - tc.Rotation;
            tc.Translation = translation;
            tc.Rotation += deltaRotation;
            tc.Scale = scale;

        }
    }



    ImGui::End();
    ImGui::PopStyleVar();

    m_Framebuffer->Unbind();
}
