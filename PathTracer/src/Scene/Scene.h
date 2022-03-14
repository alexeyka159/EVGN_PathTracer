#pragma once

#include "vendor/entt/include/entt.hpp"
#include "Components.h"

class Entity;
class Scene
{
private:
	entt::registry m_Registry;

	friend class Entity;
	friend class SceneHierarchyPanel;
	friend class Renderer;
	friend class Gravity;
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());
	void RemoveEntity(Entity& entity);

	//template<typename T>
	//entt::basic_view GetView() const
	//{
	//	auto view = m_Registry.view<T>();
	//	return *view;
	//}

	Scene& operator=(const Scene&) { return *this; }
};