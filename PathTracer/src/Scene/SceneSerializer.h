#pragma once

#include "Scene.h"
#include "Ref.h"

class SceneSerializer
{
private:
	Ref<Scene> m_Scene;

public:
	SceneSerializer(Ref<Scene> scene);

	void Serialize(const std::string& filepath);
	void SerializeRuntime(const std::string& filepath);

	bool Deserialize(const std::string& filepath);
	bool DeserializeRuntime(const std::string& filepath);
};