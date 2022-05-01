#include "SceneHierarchyPanel.h"

#include "Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>

SceneHierarchyPanel::SceneHierarchyPanel(Scene& context)
	: m_Context(&context)
{
	SetContex(context);
}

void SceneHierarchyPanel::SetContex(const Scene& context)
{
	*m_Context = context;
}

void SceneHierarchyPanel::SetSelection(const int& entityID)
{
	Entity entity = { (entt::entity)entityID, m_Context };
	m_SelectionContext = entity;
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

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		m_SelectionContext = {};

	ImGui::End();


	ImGui::Begin("Properties");
	if (m_SelectionContext)
		DrawComponents(m_SelectionContext);

	ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
	auto& tag = entity.GetComponent<TagComponent>().Tag;

	ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
	if (ImGui::IsItemClicked())
	{
		SetSelection(entity);
	}

	if (opened)
	{
		/*ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)86461, flags, tag.c_str());
		if(opened)
			ImGui::TreePop();*/
		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
		}
	}

	if (entity.HasComponent<TransformComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
		{
			auto& translation = entity.GetComponent<TransformComponent>().Translation;
			auto& scale = entity.GetComponent<TransformComponent>().Scale;
			auto& rotation = entity.GetComponent<TransformComponent>().Rotation;

			rotation = glm::degrees(rotation);

			ImGui::DragFloat3("Position", &translation[0], 0.1f);
			ImGui::DragFloat3("Scale", &scale[0], 0.1f);
			ImGui::DragFloat3("Rotation", &rotation[0], 0.1f);
			ImGui::TreePop();

			rotation = glm::radians(rotation);
		}		
	}

	if (entity.HasComponent<CameraComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
		{
			auto& camComponent = entity.GetComponent<CameraComponent>();
			auto fov = camComponent.RenderCamera->GetFov();
			auto speed = camComponent.RenderCamera->GetSpeed();
			auto& primaty = camComponent.Primary;
			auto near = camComponent.RenderCamera->GetNear();
			auto far = camComponent.RenderCamera->GetFar();

			ImGui::Checkbox("Primary camera", &primaty);
			ImGui::NewLine();
			if(ImGui::DragFloat("Near", &near, 0.1f, 0.01f))
				camComponent.RenderCamera->SetNear(near);
			if(ImGui::DragFloat("Far", &far, 0.1f))
				camComponent.RenderCamera->SetFar(far);
			ImGui::NewLine();
			if(ImGui::DragFloat("FOV", &fov, 0.1f))
				camComponent.RenderCamera->SetFov(fov);
			if(ImGui::DragFloat("Speed", &speed, 0.1f))
				camComponent.RenderCamera->SetSpeed(speed);

			ImGui::TreePop();
		}
	}
}
