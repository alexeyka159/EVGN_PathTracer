#pragma once

#include "GUIPanel.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "Ref.h"

class SceneHierarchyPanel : public GUIPanel
{
private:
    Ref<Scene> m_Context;
    Entity m_SelectionContext;

    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);

public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(Ref<Scene>& context);
    inline ~SceneHierarchyPanel() { delete this; }

    inline Ref<Scene> GetContex() { return m_Context; }
    void SetContex(const Ref<Scene>& context);
    
    inline void SetSelection(const Entity& entity) { m_SelectionContext = entity; }

    void SetSelection(const int& entityID);
    inline Entity& GetSelectedEntity() { return m_SelectionContext; };

    void Draw() override;
};

