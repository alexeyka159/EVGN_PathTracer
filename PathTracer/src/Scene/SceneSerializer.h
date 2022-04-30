#pragma once

#include "Scene.h"

class SceneSerializer
{
private:
	Scene* m_Scene;

public:
	SceneSerializer(Scene& scene);

	void Serialize(const std::string& filepath);
	void SerializeRuntime(const std::string& filepath);

	bool Deserialize(const std::string& filepath);
	bool DeserializeRuntime(const std::string& filepath);
};