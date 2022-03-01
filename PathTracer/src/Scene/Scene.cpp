#include "Scene.h"

#include <glm/glm.hpp>

#include "Entity.h"

Scene::Scene()
{
}

Scene::~Scene()
{

}

Entity Scene::CreateEntity(const std::string& name)
{
	Entity entity = { m_Registry.create(), this };

	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	return entity;
}
