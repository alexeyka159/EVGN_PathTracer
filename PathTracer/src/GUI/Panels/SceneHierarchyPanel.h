#pragma once

#include "GUIPanel.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

class SceneHierarchyPanel : public GUIPanel
{
private:
    Scene* m_Context;
    Entity m_SelectionContext;
    Entity m_FollowedByContext;
    Camera* m_Camera;
    float* m_SimulationSpeed;

    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(Scene& context, Camera& camera, float& simulatuinSpeed);
    inline ~SceneHierarchyPanel() { delete this; };

    void SetContex(const Scene& context);

    void Draw() override;
};

