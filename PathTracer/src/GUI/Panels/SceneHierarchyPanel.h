#pragma once

#include "GUIPanel.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

class SceneHierarchyPanel : public GUIPanel
{
private:
    Scene* m_Context;
    Entity m_SelectionContext;

    void DrawEntityNode(Entity entity);
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(Scene& context);
    inline ~SceneHierarchyPanel() { delete this; };

    void SetContex(const Scene& context);

    void Draw() override;
};

