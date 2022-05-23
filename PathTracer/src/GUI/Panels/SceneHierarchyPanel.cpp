#include "SceneHierarchyPanel.h"

#include "Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>

SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene>& context)
	: m_Context(context)
{
	SetContex(context);
}

void SceneHierarchyPanel::SetContex(const Ref<Scene>& context)
{
	m_Context = context;
	m_SelectionContext = {};
}

void SceneHierarchyPanel::SetSelection(const int& entityID)
{
	Entity entity = { (entt::entity)entityID, m_Context.get() };
	m_SelectionContext = entity;
}

void SceneHierarchyPanel::Draw()
{
	ImGui::Begin("Outliner");

	if (m_Context) {
		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);

				/*auto& tc = entity.GetComponent<TagComponent>();
				ImGui::Text("%s", tc.Tag.c_str());*/
			});
	}

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
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
	if (ImGui::IsItemClicked())
	{
		SetSelection(entity);
	}

	if (opened)
	{
		/*ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)86461, flags, tag.c_str());
		if(opened)
			ImGui::TreePop();*/
		ImGui::TreePop();
	}
}
template<typename T, typename UIFunction>
static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
{
	if (entity.HasComponent<T>())
	{
		ImGui::Separator();
		const ImGuiTreeNodeFlags treenodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
		auto& component = entity.GetComponent<T>();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = 25;
		//ImGui::Separator(); 
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treenodeFlags, name.c_str());
		ImGui::PopStyleVar();
		ImGui::SameLine(ImGui::GetWindowWidth() - lineHeight - 4);
		if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove Component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
			entity.RemoveComponent<T>();
	}
}

static void DrawMaterialTexture(const char* name, bool& isSet, bool& isUsing, const char* path,
								glm::vec3& col,  float& value, float& contrast, unsigned int texId)
{
	ImGui::Text(name);
	ImGui::SameLine(ImGui::GetWindowWidth() - 75);
	if (ImGui::Button("Browse..."))
	{

	}
	
	if (name != "Normal map:") {
		std::string title = std::string("Color##") + std::to_string(texId);
		ImGui::ColorEdit3(title.c_str(), glm::value_ptr(col), 0);
	}

	if (isSet)
	{
		std::string title = std::string("Use Texture##") + std::to_string(texId);
		ImGui::Checkbox(title.c_str(), &isUsing);
		ImGui::Image((void*)texId, ImVec2{ 80, 80 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Image((void*)texId, ImVec2{ 350, 350 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::EndTooltip();
		}
		ImGui::SameLine();

		ImGui::TextWrapped(path);
	}

	if (name != "Normal map:")
	{
		std::string title = std::string("Value##") + std::to_string(texId);
		ImGui::SliderFloat(title.c_str(), &value, -10.0f, 10.0f);
		title = std::string("Contrast##") + std::to_string(texId);
		ImGui::SliderFloat(title.c_str(), &contrast, 0.f, 10.0f);
	}
	else
	{
		std::string title = std::string("Strength##") + std::to_string(texId);
		ImGui::SliderFloat(title.c_str(), &value, 0.f, 10.0f);
	}

	ImGui::Separator();
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
		}
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");
	
	if (ImGui::BeginPopup("AddComponent"))
	{
		if(ImGui::MenuItem("Camera"))
		{
			//m_SelectionContext.AddComponent<CameraComponent>();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Model"))
		{
			//m_SelectionContext.AddComponent<ModelRendererComponent>("");
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Point Light"))
		{
			m_SelectionContext.AddComponent<PointLightComponent>();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			auto& translation = component.Translation;
			auto& scale = component.Scale;
			auto& rotation = component.Rotation;

			rotation = glm::degrees(rotation);

			ImGui::DragFloat3("Position", &translation[0], 0.1f);
			ImGui::DragFloat3("Scale", &scale[0], 0.1f);
			ImGui::DragFloat3("Rotation", &rotation[0], 0.1f);

			rotation = glm::radians(rotation);
		});

	
	DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto fov = component.RenderCamera->GetFov();
			//auto speed = component.RenderCamera->GetSpeed();
			auto& primaty = component.Primary;
			auto near = component.RenderCamera->GetNear();
			auto far = component.RenderCamera->GetFar();

			ImGui::Checkbox("Primary camera", &primaty);
			ImGui::NewLine();
			if (ImGui::DragFloat("Near", &near, 0.1f, 0.01f))
				component.RenderCamera->SetNear(near);
			if (ImGui::DragFloat("Far", &far, 0.1f))
				component.RenderCamera->SetFar(far);
			ImGui::NewLine();
			if (ImGui::DragFloat("FOV", &fov, 0.1f))
				component.RenderCamera->SetFov(fov);
		});

	DrawComponent<ModelRendererComponent>("Model", entity, [](auto& component)
		{
			std::string pathStr = "Path: " + component.ModelObj.GetPath();
			ImGui::Text(pathStr.c_str());
			ImGui::Spacing();


			const ImGuiTreeNodeFlags innerTreenodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (ImGui::TreeNodeEx((void*)typeid(ModelRendererComponent).hash_code(), innerTreenodeFlags, "Material"))
			{
				Material& mat = component.ModelObj.GetMaterial();
				std::vector<Texture>& textures = mat.TexturesLoaded;
				
				bool diffSet = false;
				std::string diffPath = "";
				unsigned int diffId = 0;

				bool roughSet = false;
				std::string roughPath = "";
				unsigned int roughId = 0;

				bool normSet = false;
				std::string normPath = "";
				unsigned int normId = 0;

				for (Texture& tex : textures)
				{
					switch (tex.GetType())
					{
						case Texture::TextureType::DIFFUSE:		diffSet = true;	 diffPath = tex.GetPath(); diffId = tex.GetId(); break;
						case Texture::TextureType::ROUGHNESS:	roughSet = true; roughPath = tex.GetPath(); roughId = tex.GetId(); break;
						case Texture::TextureType::NORMAL:		normSet = true;  normPath  = tex.GetPath(); normId = tex.GetId(); break;
					}
				}
				
				DrawMaterialTexture("Diffuse map:", diffSet, mat.IsDiffuseUsing, diffPath.c_str(), mat.DiffuseColor,
					mat.DiffuseValue, mat.DiffuseContrast, diffId);
				DrawMaterialTexture("Roughness map:", roughSet, mat.IsRoughnessUsing, roughPath.c_str(), mat.RoughnessColor,
					mat.RoughnessValue, mat.RoughnessContrast, roughId);
				DrawMaterialTexture("Normal map:", normSet, mat.IsNormalUsing, normPath.c_str(), mat.DiffuseColor,
					mat.NormalStrength, mat.DiffuseContrast, normId);

				ImGui::TreePop();
			}

		});


	DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component)
		{
			ImGui::DragFloat("Intensity", &component.Intensity, 0.025f, 0);
			ImGui::ColorEdit3("Color", &component.Color[0], 0);
			/*ImGui::DragFloat("Constant", &component.Constant, 0.025f, 0);
			ImGui::DragFloat("Linear", &component.Linear, 0.025f, 0);
			ImGui::DragFloat("Quadratic", &component.Quadratic, 0.025f, 0);*/
		});

	DrawComponent<SpotLightComponent>("Spot Light", entity, [](auto& component)
		{
			float cutOff = -component.CutOff;
			float outerCutOff = -component.OuterCutOff;
			ImGui::DragFloat("Intensity", &component.Intensity, 0.025f, 0);
			ImGui::ColorEdit3("Color", &component.Color[0], 0);
			if (ImGui::DragFloat("CutOff", &cutOff, 0.025f, 0))
				component.CutOff = -cutOff;
			if (ImGui::DragFloat("Outer CutOff", &outerCutOff, 0.025f, 0))
				component.OuterCutOff = -outerCutOff;
			/*ImGui::DragFloat("Constant", &component.Constant, 0.025f, 0);
			ImGui::DragFloat("Linear", &component.Linear, 0.025f, 0);
			ImGui::DragFloat("Quadratic", &component.Quadratic, 0.025f, 0);*/
		});

	DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component)
		{
			ImGui::DragFloat("Intensity", &component.Intensity, 0.025f, 0);
			ImGui::ColorEdit3("Color", &component.Color[0], 0);
		});
}
