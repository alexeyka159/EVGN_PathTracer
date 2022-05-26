#include "SceneSerializer.h"

#include "Entity.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;

		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;

		}
	};
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

SceneSerializer::SceneSerializer(Ref<Scene> scene)
	: m_Scene(scene)
{
}

static void SerializeEntity(YAML::Emitter& out, Entity entity)
{
	out << YAML::BeginMap; //Entity
	out << YAML::Key << "Entity" << YAML::Value << "123123123";

	if (entity.HasComponent<TagComponent>())
	{
		out << YAML::Key << "TagComponent";
		out << YAML::BeginMap; //TagComponent

		auto& tag = entity.GetComponent<TagComponent>().Tag;
		out << YAML::Key << "Tag" << YAML::Value << tag;

		out << YAML::EndMap; //TagComponent
	}

	if (entity.HasComponent<TransformComponent>())
	{
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap; //TransformComponent

		auto& tc = entity.GetComponent<TransformComponent>();
		out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
		out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
		out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

		out << YAML::EndMap; //TransformComponent
	}

	if (entity.HasComponent<CameraComponent>())
	{
		out << YAML::Key << "CameraComponent";
		out << YAML::BeginMap; //CameraComponent
		auto& cc = entity.GetComponent<CameraComponent>();
		auto& camera = cc.RenderCamera;
		out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
		out << YAML::Key << "Position" << YAML::Value << camera->GetPosition(); //Временно
		out << YAML::Key << "Fov" << YAML::Value << camera->GetFov();
		out << YAML::Key << "Far" << YAML::Value << camera->GetFar();
		out << YAML::Key << "Near" << YAML::Value << camera->GetNear();
		out << YAML::Key << "Direction" << YAML::Value << camera->GetDirection();
		out << YAML::Key << "Speed" << YAML::Value << camera->GetSpeed();

		out << YAML::EndMap; //CameraComponent
	}

	if (entity.HasComponent<ModelRendererComponent>())
	{
		out << YAML::Key << "ModelRendererComponent";
		out << YAML::BeginMap; //ModelRendererComponent
		auto& modelComponent = entity.GetComponent<ModelRendererComponent>();
		out << YAML::Key << "Path" << YAML::Value << modelComponent.ModelObj.GetPath();
		
		auto& mat = modelComponent.ModelObj.GetMaterial();
		out << YAML::Key << "DiffuseColor"		<< YAML::Value << mat.DiffuseColor;
		out << YAML::Key << "IsDiffuseUsing"	<< YAML::Value << mat.IsDiffuseUsing;
		out << YAML::Key << "DiffuseValue"		<< YAML::Value << mat.DiffuseValue;
		out << YAML::Key << "DiffuseContrast"	<< YAML::Value << mat.DiffuseContrast;
		out << YAML::Key << "IsMetallicUsing"	<< YAML::Value << mat.IsMetallicUsing;
		out << YAML::Key << "MetallicColor"		<< YAML::Value << mat.MetallicColor;
		out << YAML::Key << "RoughnessColor"	<< YAML::Value << mat.RoughnessColor;
		out << YAML::Key << "IsRoughnessUsing"	<< YAML::Value << mat.IsRoughnessUsing;
		out << YAML::Key << "IsRoughnessInvert" << YAML::Value << mat.IsRoughnessInvert;
		out << YAML::Key << "RoughnessValue"	<< YAML::Value << mat.RoughnessValue;
		out << YAML::Key << "RoughnessContrast" << YAML::Value << mat.RoughnessContrast;
		out << YAML::Key << "IsNormalUsing"		<< YAML::Value << mat.IsNormalUsing;
		out << YAML::Key << "NormalStrength"	<< YAML::Value << mat.NormalStrength;

		out << YAML::EndMap; //ModelRendererComponent
	}

	if (entity.HasComponent<PointLightComponent>())
	{
		out << YAML::Key << "PointLightComponent";
		out << YAML::BeginMap; //PointLightComponent
		auto& lightComponent = entity.GetComponent<PointLightComponent>();
		out << YAML::Key << "Intensity" << YAML::Value << lightComponent.Intensity;
		out << YAML::Key << "Color" << YAML::Value << lightComponent.Color;

		out << YAML::Key << "Constant" << YAML::Value << lightComponent.Constant;
		out << YAML::Key << "Linear" << YAML::Value << lightComponent.Linear;
		out << YAML::Key << "Quadratic" << YAML::Value << lightComponent.Quadratic;

		out << YAML::EndMap; //PointLightComponent
	}

	if (entity.HasComponent<SpotLightComponent>())
	{
		out << YAML::Key << "SpotLightComponent";
		out << YAML::BeginMap; //SpotLightComponent
		auto& lightComponent = entity.GetComponent<SpotLightComponent>();
		out << YAML::Key << "Intensity" << YAML::Value << lightComponent.Intensity;
		out << YAML::Key << "Color" << YAML::Value << lightComponent.Color;

		out << YAML::Key << "CutOff" << YAML::Value << lightComponent.CutOff;
		out << YAML::Key << "OuterCutOff" << YAML::Value << lightComponent.OuterCutOff;

		out << YAML::Key << "Constant" << YAML::Value << lightComponent.Constant;
		out << YAML::Key << "Linear" << YAML::Value << lightComponent.Linear;
		out << YAML::Key << "Quadratic" << YAML::Value << lightComponent.Quadratic;

		out << YAML::EndMap; //SpotLightComponent
	}

	if (entity.HasComponent<DirectionalLightComponent>())
	{
		out << YAML::Key << "DirectionalLightComponent";
		out << YAML::BeginMap; //DirectionalLightComponent
		auto& lightComponent = entity.GetComponent<DirectionalLightComponent>();
		out << YAML::Key << "Intensity" << YAML::Value << lightComponent.Intensity;
		out << YAML::Key << "Color" << YAML::Value << lightComponent.Color;

		out << YAML::EndMap; //DirectionalLightComponent
	}

	out << YAML::EndMap; //Entity
}

void SceneSerializer::Serialize(const std::string& filepath)
{
	std::string sceneName = "Untitled";
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << sceneName;
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

	m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		});
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
	std::cout << "Scene \"" << sceneName << "\" saved to \"" << filepath << "\"\n";
}

void SceneSerializer::SerializeRuntime(const std::string& filepath)
{
}

bool SceneSerializer::Deserialize(const std::string& filepath)
{
	std::ifstream stream(filepath);
	std::stringstream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["Scene"])
		return false;

	std::string sceneName = data["Scene"].as<std::string>();
	std::cout << "Loading scene \"" << sceneName << "\" from \"" << filepath << "\"..." << std::endl;

	auto entities = data["Entities"];
	if (entities)
	{
		for (auto entity : entities)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();

			std::string name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent)
				name = tagComponent["Tag"].as<std::string>();

			std::cout << "Loading entity with ID = " << uuid << ", name = " << name << "..." << std::endl;

			Entity deserializedEntity = m_Scene->CreateEntity(name);

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				auto& tc		= deserializedEntity.GetComponent<TransformComponent>();
				tc.Translation	= transformComponent["Translation"].as<glm::vec3>();
				tc.Rotation		= transformComponent["Rotation"].as<glm::vec3>();
				tc.Scale		= transformComponent["Scale"].as<glm::vec3>();

			}

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				bool primary		= cameraComponent["Primary"].as<bool>();
				glm::vec3 position	= cameraComponent["Position"].as<glm::vec3>();
				float fov			= cameraComponent["Fov"].as<float>();
				float far			= cameraComponent["Far"].as<float>();
				float near			= cameraComponent["Near"].as<float>();
				glm::vec3 direction = cameraComponent["Direction"].as<glm::vec3>();
				float speed			= cameraComponent["Speed"].as<float>();

				deserializedEntity.AddComponent<CameraComponent>(position, direction, glm::vec3(0.f, 1.f, 0.f), fov, near, far, speed, primary);
			}

			auto modelComponent = entity["ModelRendererComponent"];
			if (modelComponent)
			{
				std::string path = modelComponent["Path"].as<std::string>();
				std::cout << "  - Loading model: \"" << path << "\"\n";
				deserializedEntity.AddComponent<ModelRendererComponent>(path.c_str());

				auto& mat = deserializedEntity.GetComponent<ModelRendererComponent>().ModelObj.GetMaterial();
				mat.IsDiffuseUsing		= modelComponent["IsDiffuseUsing"].as<bool>();
				mat.DiffuseColor		= modelComponent["DiffuseColor"].as<glm::vec3>();
				mat.DiffuseValue		= modelComponent["DiffuseValue"].as<float>();
				mat.DiffuseContrast		= modelComponent["DiffuseContrast"].as<float>();
				mat.IsMetallicUsing		= modelComponent["IsMetallicUsing"].as<bool>();
				mat.MetallicColor		= modelComponent["MetallicColor"].as<float>();
				mat.IsRoughnessUsing	= modelComponent["IsRoughnessUsing"].as<bool>();
				mat.IsRoughnessInvert	= modelComponent["IsRoughnessInvert"].as<bool>();
				mat.RoughnessColor		= modelComponent["RoughnessColor"].as<glm::vec3>();
				mat.RoughnessValue		= modelComponent["RoughnessValue"].as<float>();
				mat.RoughnessContrast	= modelComponent["RoughnessContrast"].as<float>();
				mat.IsNormalUsing		= modelComponent["IsNormalUsing"].as<bool>();
				mat.NormalStrength		= modelComponent["NormalStrength"].as<float>();
			}

			auto pointLightComponent = entity["PointLightComponent"];
			if (pointLightComponent)
			{
				auto& pl = deserializedEntity.AddComponent<PointLightComponent>();
				pl.Intensity	= pointLightComponent["Intensity"].as<float>();
				pl.Color		= pointLightComponent["Color"].as<glm::vec3>();
				pl.Constant		= pointLightComponent["Constant"].as<float>();
				pl.Linear		= pointLightComponent["Linear"].as<float>();
				pl.Quadratic	= pointLightComponent["Quadratic"].as<float>();
			}

			auto spotLightComponent = entity["SpotLightComponent"];
			if (spotLightComponent)
			{
				auto& sl = deserializedEntity.AddComponent<SpotLightComponent>();
				sl.Intensity	= spotLightComponent["Intensity"].as<float>();
				sl.Color		= spotLightComponent["Color"].as<glm::vec3>();
				
				sl.CutOff		= spotLightComponent["CutOff"].as<float>();
				sl.OuterCutOff	= spotLightComponent["OuterCutOff"].as<float>();
				
				sl.Constant		= spotLightComponent["Constant"].as<float>();
				sl.Linear		= spotLightComponent["Linear"].as<float>();
				sl.Quadratic	= spotLightComponent["Quadratic"].as<float>();
			}

			auto directionalLightComponent = entity["DirectionalLightComponent"];
			if (directionalLightComponent)
			{
				auto& dl = deserializedEntity.AddComponent<DirectionalLightComponent>();
				dl.Intensity	= directionalLightComponent["Intensity"].as<float>();
				dl.Color		= directionalLightComponent["Color"].as<glm::vec3>();
			}

			std::cout << std::endl;
		}
	}

	return true;
}

bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
{
	return false;
}
