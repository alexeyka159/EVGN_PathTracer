#include "SceneHierarchyPanel.h"

#include "Utils/WindowsPlatformUtils.h"
#include "Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>

SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene>& context, GLFWwindow& window)
	: m_Context(context)
	, m_Window(&window)
	, m_Environment(std::make_shared<EnvironmentMap>())
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


	ImGui::Begin("Render output");
	DrawOutputProperties();
	ImGui::End();

	ImGui::Begin("Object");
	if (m_SelectionContext)
		DrawComponents(m_SelectionContext);
	ImGui::End();

	ImGui::Begin("World");
	DrawWorldProperties();
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
								glm::vec3& col,  float& value, float& contrast, unsigned int texId, bool* inverted = nullptr)
{
	ImGui::Text(name);
	ImGui::SameLine(ImGui::GetWindowWidth() - 75);
	if (ImGui::Button("Browse..."))
	{

	}
	
	if (name != "Normal map:" && name != "Metallic map:") {
		std::string title = std::string("Color##") + name;
		ImGui::ColorEdit3(title.c_str(), glm::value_ptr(col), 0);
	}

	if (isSet)
	{
		std::string title = std::string("Use Texture##") + name;
		ImGui::Checkbox(title.c_str(), &isUsing);
		if (name == "Roughness map:" && isUsing)
		{
			title = std::string("Invert##") + name;
			ImGui::Checkbox(title.c_str(), inverted);
		}
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
		std::string title = std::string("Value##") + name;
		ImGui::SliderFloat(title.c_str(), &value, .0f, name == "Metallic map:" ? 1.0f : 5.f, "%.03f");
		if (name != "Metallic map:")
		{
			title = std::string("Contrast##") + name;
			ImGui::SliderFloat(title.c_str(), &contrast, 0.f, 2.0f, "%.03f");
		}
	}
	else
	{
		std::string title = std::string("Strength##") + name;
		ImGui::SliderFloat(title.c_str(), &value, 0.f, 10.0f, "%.03f");
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

	DrawComponent<ModelRendererComponent>("Model", entity, [&entity](auto& component)
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

				bool metalSet = false;
				std::string metalPath = "";
				unsigned int metalhId = 0;

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
						case Texture::TextureType::METALLIC:	metalSet = true; metalPath = tex.GetPath(); metalhId = tex.GetId(); break;
						case Texture::TextureType::ROUGHNESS:	roughSet = true; roughPath = tex.GetPath(); roughId = tex.GetId(); break;
						case Texture::TextureType::NORMAL:		normSet = true;  normPath  = tex.GetPath(); normId = tex.GetId(); break;
					}
				}

				ImGui::SliderFloat("IOR", &mat.IOR, 0, 1, "%.03f");
				ImGui::Separator();

				DrawMaterialTexture("Diffuse map:", diffSet, mat.IsDiffuseUsing, diffPath.c_str(), mat.DiffuseColor,
					mat.DiffuseValue, mat.DiffuseContrast, diffId);
				DrawMaterialTexture("Metallic map:", metalSet, mat.IsMetallicUsing, metalPath.c_str(), mat.RoughnessColor,
					mat.MetallicColor, mat.RoughnessContrast, metalhId);
				DrawMaterialTexture("Roughness map:", roughSet, mat.IsRoughnessUsing, roughPath.c_str(), mat.RoughnessColor,
					mat.RoughnessValue, mat.RoughnessContrast, roughId, &mat.IsRoughnessInvert);
				DrawMaterialTexture("Normal map:", normSet, mat.IsNormalUsing, normPath.c_str(), mat.DiffuseColor,
					mat.NormalStrength, mat.DiffuseContrast, normId);

				ImGui::TreePop();
			}

		});


	DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component)
		{
			ImGui::DragFloat("Intensity", &component.Intensity, 0.025f, 0);
			ImGui::ColorEdit3("Color", &component.Color[0], 0);
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
		});

	DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component)
		{
			ImGui::DragFloat("Intensity", &component.Intensity, 0.025f, 0);
			ImGui::ColorEdit3("Color", &component.Color[0], 0);
		});
}

void SceneHierarchyPanel::DrawWorldProperties()
{
	const ImGuiTreeNodeFlags innerTreenodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (ImGui::TreeNodeEx((void*)typeid(EnvironmentMap).hash_code(), innerTreenodeFlags, "Environment map"))
	{
		EnvironmentProperties& envirProperties = m_Environment->GetProperties();

		ImGui::SameLine(ImGui::GetWindowWidth() - 66);
		if (ImGui::Button("Browse..."))
		{
			std::string filepath = FileDialogs::OpenFile("Environment (*.hdri, *.hdr)\0*.hdri;*.hdr\0", m_Window);

			if (!filepath.empty())
			{
				m_Environment->LoadEnvironmentMap(filepath.c_str());
				envirProperties.IsEnvironmentMapUsing = true;
			}
		}

		ImGui::ColorEdit3("Color", glm::value_ptr(envirProperties.Color));
		

		if (envirProperties.IsSet)
		{
			ImGui::Checkbox("Draw background", &envirProperties.IsDrawingBackground);
			ImGui::Checkbox("IBL", &envirProperties.IsEnvironmentMapUsing);

			if (envirProperties.IsDrawingBackground || envirProperties.IsEnvironmentMapUsing)
			{
				ImGui::SliderFloat("Blur", &envirProperties.Blur, 0, 5);
				ImGui::SliderFloat("Rotation", &envirProperties.Rotation, 0, 360);
				ImGui::SliderFloat("Intensity", &envirProperties.Intensity, 0, 5);
			}

			glm::vec2 imgSize = m_Environment->GetSize();
			float imgMaxWidth = ImGui::GetContentRegionAvail().x - 10;
			if (imgMaxWidth >= 450)
				imgMaxWidth = 450;
			int aspect = imgSize.x / imgMaxWidth;
			ImGui::Image((void*)m_Environment->GetHDRI(), ImVec2{imgSize.x / aspect, imgSize.y / aspect}, ImVec2{0, 1}, ImVec2{1, 0});
			if (ImGui::IsItemHovered())
			{
				imgMaxWidth = 700;
				ImGui::BeginTooltip();
				aspect = imgSize.x / imgMaxWidth;
				ImGui::Image((void*)m_Environment->GetHDRI(), ImVec2{ imgSize.x / aspect, imgSize.y / aspect }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				ImGui::EndTooltip();
			}
		}
		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::DrawOutputProperties()
{

}
