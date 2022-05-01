#include "SceneSerializer.h"

#include "Entity.h"
#include "Camera/CameraOrbit.h"

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

SceneSerializer::SceneSerializer(Scene& scene)
	: m_Scene(&scene)
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

		out << YAML::EndMap; //ModelRendererComponent
	}

	out << YAML::EndMap; //Entity

}

void SceneSerializer::Serialize(const std::string& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

	m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		});
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
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
	std::cout << "\nDeserializing scene \"" << sceneName << "\"" << std::endl;

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

			std::cout << "Deserialized entity with ID = " << uuid << ", name = " << name << std::endl;

			Entity deserializedEntity = m_Scene->CreateEntity(name);

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				auto& tc		= deserializedEntity.GetComponent<TransformComponent>();
				tc.Translation	= transformComponent["Translation"].as<glm::vec3>();
				tc.Rotation		= transformComponent["Rotation"].as<glm::vec3>();
				tc.Scale		= transformComponent["Scale"].as<glm::vec3>();

			}

			/*auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				bool primary		= cameraComponent["Primary"].as<bool>();
				glm::vec3 position	= cameraComponent["Position"].as<glm::vec3>();
				float fov			= cameraComponent["Fov"].as<float>();
				float far			= cameraComponent["Far"].as<float>();
				float near			= cameraComponent["Near"].as<float>();
				glm::vec3 direction = cameraComponent["Direction"].as<glm::vec3>();
				float speed			= cameraComponent["Speed"].as<float>();

				CameraOrbit camera(position, direction, glm::vec3(0.f, 1.f, 0.f), fov, near, far, speed);
				deserializedEntity.AddComponent<CameraComponent>(camera, primary);
			}*/

			auto modelComponent = entity["ModelRendererComponent"];
			if (modelComponent)
			{
				std::string path = modelComponent["Path"].as<std::string>();
				std::cout << "  - Loading model: \"" << path << "\"\n";
				deserializedEntity.AddComponent<ModelRendererComponent>(path.c_str());
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
