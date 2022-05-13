#include "ViewportPanel.h"

#include <iostream>
#include "glm/gtc/type_ptr.hpp"

#include "Math/Math.h"

#include "Scene/SceneSerializer.h"

#include "Utils/WindowsPlatformUtils.h"
#include "SceneHierarchyPanel.h"

ViewportPanel::ViewportPanel(Framebuffer& framebuffer, GLFWwindow& window, SceneHierarchyPanel& sceneHierarchyPanel, Camera& camera)
    : m_Framebuffer(&framebuffer)
    , m_GuiTextureID(0)
    , m_Camera(&camera)
    , m_Window(&window)
    , m_SceneHierarchyPanel(&sceneHierarchyPanel)
    , m_OutputType(0)
{
}

void ViewportPanel::Draw()
{
    m_Framebuffer->Bind();

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
                NewScene();
            if (ImGui::MenuItem("Open...", "Ctrl+O"))
                OpenScene();
            if(ImGui::MenuItem("Save", "Ctrl+S"))
                SaveScene();
            if(ImGui::MenuItem("Save as...", "Ctrl+Shift+S"))
                SaveAsScene();
            if (ImGui::BeginMenu("Import...", "Ctrl+I"))
            {
                if (ImGui::MenuItem("FBX (.fbx)"))
                    ImportModel("FBX Model (*.fbx)\0*.fbx\0");

                if (ImGui::MenuItem("Wavefront (.obj)"))
                    ImportModel("Obj Model (*.obj)\0*.obj\0");
                
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::MenuItem("Add", "Ctrl+Shift+A");
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Color"))
            m_OutputType = 0;
        if (ImGui::MenuItem("Depth"))
            m_OutputType = 2;

        ImGui::EndMenuBar();

        
    }

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

    //Хоткеи Файла
    if (glfwGetKey(m_Window, GLFW_KEY_O) == GLFW_PRESS && glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        OpenScene();

    if (glfwGetKey(m_Window, GLFW_KEY_N) == GLFW_PRESS && glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        NewScene();

    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
        glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        SaveScene();

    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
        glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        SaveAsScene();

    if (glfwGetKey(m_Window, GLFW_KEY_I) == GLFW_PRESS && glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        ImGui::OpenPopup("Import");

    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE &&
        glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        ImGui::OpenPopup("AddMenu");


    if (ImGui::BeginPopup("Import"))
    {
        if (ImGui::MenuItem("FBX (.fbx)"))
            ImportModel("FBX Model (*.fbx)\0*.fbx\0");

        if (ImGui::MenuItem("Wavefront (.obj)"))
            ImportModel("Obj Model (*.obj)\0*.obj\0");

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("AddMenu"))
    {
        ImGui::Text("Add");
        ImGui::Separator();

        if (ImGui::BeginMenu("Mesh"))
        {
            if (ImGui::MenuItem("Plane"))
            {
            }
            
            if (ImGui::MenuItem("Cube"))
            {
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Import...", "Ctrl+I"))
            {
                if (ImGui::MenuItem("FBX (.fbx)"))
                    ImportModel("FBX Model (*.fbx)\0*.fbx\0");

                if (ImGui::MenuItem("Wavefront (.obj)"))
                    ImportModel("Obj Model (*.obj)\0*.obj\0");
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Light"))
        {
            if (ImGui::MenuItem("Point"))
            {
                Entity light = m_SceneHierarchyPanel->GetContex()->CreateEntity("Point Light");
                light.AddComponent<PointLightComponent>();
            }

            if (ImGui::MenuItem("Spot"))
            {
                Entity light = m_SceneHierarchyPanel->GetContex()->CreateEntity("Spot Light");
                light.AddComponent<SpotLightComponent>();
            }

            if (ImGui::MenuItem("Sun"))
            {
                Entity light = m_SceneHierarchyPanel->GetContex()->CreateEntity("Sun Light");
                light.AddComponent<DirectionalLightComponent>();
            }

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }

    //Хоткеи сцены
    if (glfwGetKey(m_Window, GLFW_KEY_X) == GLFW_PRESS || glfwGetKey(m_Window, GLFW_KEY_DELETE) == GLFW_PRESS)
    {
        Entity& selectedContext = m_SceneHierarchyPanel->GetSelectedEntity();
        if(selectedContext)
            ImGui::OpenPopup("DeleteEntity");
    }
    if (ImGui::BeginPopup("DeleteEntity"))
    {
        ImGui::Text("OK?");
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
        {
            Entity& selectedContext = m_SceneHierarchyPanel->GetSelectedEntity();
            m_SceneHierarchyPanel->GetContex()->RemoveEntity(selectedContext);
            m_SceneHierarchyPanel->SetSelection(-1);
        }

        ImGui::EndPopup();
    }

    m_GuiTextureID = m_Framebuffer->GetColorAttachmentRendererId(m_OutputType);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");
    auto viewportOffset = ImGui::GetCursorPos();

    ImVec2 viewportPanelSize = ImGui::GetContentRegionMax();
    
    if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
    {
        m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    }
    ImGui::Image((void*)m_GuiTextureID, ImVec2{ viewportPanelSize.x, viewportPanelSize.y -23 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
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


        glm::mat4 cameraProjection = m_Camera->GetProjection();
        glm::mat4 cameraView = m_Camera->GetViewMatrix();

        auto& tc = selectedEntity.GetComponent<TransformComponent>();
        glm::mat4 transform = tc.GetTransform();

        float snapValue = 0.5f;
        if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 15.f;

        float snapValues[3] = { snapValue, snapValue, snapValue};

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
            (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
            nullptr, snap ? snapValues : nullptr);

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation, scale, rotation;

            Math::DecomposeTransform(transform, translation, rotation, scale);

            tc.Translation = translation;
            tc.Rotation = rotation;
            tc.Scale = scale;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

    m_Framebuffer->Unbind();
}

void ViewportPanel::SaveScene()
{
    if (!m_LastScenePath.empty())
    {
        SceneSerializer serializer(m_SceneHierarchyPanel->GetContex());
        serializer.Serialize(m_LastScenePath);
    }
    else
        SaveAsScene();
}

void ViewportPanel::OpenScene()
{
    std::string filepath = FileDialogs::OpenFile("Evergreen Scene (*.evgn)\0*.evgn\0", m_Window);

    if (!filepath.empty())
    {
        Ref<Scene> newScene = std::make_shared<Scene>();
        m_SceneHierarchyPanel->SetContex(newScene);

        SceneSerializer serializer(m_SceneHierarchyPanel->GetContex());
        serializer.Deserialize(filepath);
        m_LastScenePath = filepath;
    }
}

void ViewportPanel::SaveAsScene()
{
    std::string filepath = FileDialogs::SaveFile("Evergreen Scene (*.evgn)\0*.evgn\0", m_Window);

    if (!filepath.empty())
    {
        SceneSerializer serializer(m_SceneHierarchyPanel->GetContex());
        serializer.Serialize(filepath);
        m_LastScenePath = filepath;
    }
}

void ViewportPanel::NewScene()
{
    Ref<Scene> newScene = std::make_shared<Scene>();
    m_SceneHierarchyPanel->SetContex(newScene);
    m_LastScenePath = "";
}

void ViewportPanel::ImportModel(const char* filter)
{
    std::string filepath = FileDialogs::SaveFile(filter, m_Window);

    if (!filepath.empty())
    {
        Entity impEntity = m_SceneHierarchyPanel->GetContex()->CreateEntity();
        impEntity.AddComponent<ModelRendererComponent>(filepath.c_str());
        impEntity.GetComponent<TagComponent>().Tag = impEntity.GetComponent<ModelRendererComponent>().ModelObj.GetName();
    }
}
