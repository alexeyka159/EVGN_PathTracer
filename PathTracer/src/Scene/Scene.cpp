#include "Scene.h"

#include <glm/glm.hpp>

#include "Entity.h"

Scene::Scene()
{
	//struct MeshComponent
	//{
	//	bool Data;
	//	MeshComponent() = default;
	//};
	//struct TransformComponent
	//{
	//	glm::mat4 Transform;

	//	TransformComponent() = default;
	//	TransformComponent(const TransformComponent&) = default;
	//	TransformComponent(const glm::mat4& transform)
	//		: Transform(transform) {}

	//	operator glm::mat4& () { return Transform; }
	//	operator const glm::mat4& () const { return Transform; }
	//};


	//entt::entity entity = m_Registry.create();
	//m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

	//if(m_Registry.any_of<TransformComponent>(entity))
	//	TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

	//auto view = m_Registry.view<TransformComponent>();
	//for (auto entity : view)
	//{
	//	//view.get<TransformComponent>(entity);
	//	TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
	//}

	//auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
	//for (auto entity : group)
	//{
	//	//auto& [a, b] = { 1, 2 };
	//	//auto gr = group.get<TransformComponent, MeshComponent>(entity);
	//}
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
