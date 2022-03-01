#pragma once

#include "Scene.h"
#include "vendor/entt/include/entt.hpp"

class Entity
{
private:
	entt::entity m_EntityHandle = (entt::entity)-1;
	Scene* m_Scene = nullptr;

public:
	Entity() = default;
	Entity(const Entity& other) = default;
	Entity(entt::entity handle, Scene* scene);

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		if (HasComponent<T>())
		{
			//std::cin.get();
			throw std::exception("Entity already has component!");
			//return nullptr;
		}

		return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
	}
	template<typename T>
	T& GetComponent()
	{
		if (!HasComponent<T>())
		{
			//std::cin.get();
			throw std::exception("Entity does not have component!");
			//return nullptr;
		}

		return m_Scene->m_Registry.get<T>(m_EntityHandle);
	}

	template<typename T>
	void RemoveComponent()
	{
		if (HasComponent<T>())
		{
			//std::cin.get();
			throw std::exception("Entity does not have component!");
			//return nullptr;
		}

		m_Scene->m_Registry.remove<T>(m_EntityHandle);
	}

	template<typename T>
	bool HasComponent()
	{
		return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
	}

	operator bool() const { return m_EntityHandle != (entt::entity)-1; }
};