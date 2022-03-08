#include "SceneHierarchyPanel.h"

#include "Scene/Components.h"

SceneHierarchyPanel::SceneHierarchyPanel(Scene& context)
	: m_Context(&context)
{
	SetContex(context);
}

void SceneHierarchyPanel::SetContex(const Scene& context)
{
	*m_Context = context;
}

void SceneHierarchyPanel::Draw()
{
	ImGui::Begin("Outliner");

	m_Context->m_Registry.each([&](auto entityID)
	{
		Entity entity{ entityID, m_Context };
		DrawEntityNode(entity);

		/*auto& tc = entity.GetComponent<TagComponent>();
		ImGui::Text("%s", tc.Tag.c_str());*/
	});

	ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
	auto& tag = entity.GetComponent<TagComponent>().Tag;

	ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
	if (ImGui::IsItemClicked())
	{
		m_SelectionContext = entity;
	}

	if (opened)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)86461, flags, tag.c_str());
		if(opened)
			ImGui::TreePop();
		ImGui::TreePop();
	}
}
