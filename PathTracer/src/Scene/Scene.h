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
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());

	Scene& operator=(const Scene&) { return *this; }
};