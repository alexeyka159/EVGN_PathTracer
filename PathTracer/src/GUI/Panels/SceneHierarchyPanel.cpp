#include "SceneHierarchyPanel.h"

#include "Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>

#include "Camera/Camera.h"

SceneHierarchyPanel::SceneHierarchyPanel(Scene& context, Camera& camera)
	: m_Context(&context)
	, m_Camera(&camera)
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

	if (m_FollowedByContext) {
		const glm::vec3 lookAt = m_FollowedByContext.GetComponent<TransformComponent>().Transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		m_Camera->SetCameraView( (lookAt - m_Camera->GetPosition()), lookAt, glm::vec3(0, 1, 0));
	}

	ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
	if (ImGui::IsItemClicked())
	{
		m_SelectionContext = entity;
	}

	if (opened)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
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
		if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
		}
	}

	

	if (entity.HasComponent<TransformComponent>())
	{
		if (ImGui::Button("Focus on"))
			m_FollowedByContext = m_SelectionContext;
		ImGui::SameLine();
		if (ImGui::Button("Stop focusing"))
			m_FollowedByContext = { };
		if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
		{
			auto& transform = entity.GetComponent<TransformComponent>().Transform;

			ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);
			ImGui::TreePop();
		}		
	}

	if (entity.HasComponent<ModelRendererComponent>())
	{
		
	}

	if (entity.HasComponent<GravityComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(GravityComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Physics"))
		{
			auto& mass = entity.GetComponent<GravityComponent>().Mass;
			auto& radius = entity.GetComponent<GravityComponent>().Radius;
			auto& velocity = entity.GetComponent<GravityComponent>().CurrentVelocity;

			ImGui::DragFloat("Mass", &mass, 0.1f, 0.1f);
			ImGui::DragFloat("Radius", &radius, 0.1f);
			ImGui::DragFloat3("Velocity", &velocity[0], 0.1f);

			if (entity.HasComponent<TrailComponent>())
			{
				if (ImGui::TreeNodeEx((void*)typeid(TrailComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Trail"))
				{
					auto& color = entity.GetComponent<TrailComponent>().Color;

					ImGui::ColorPicker4("##picker", (float*)&color);

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		
	}
}
