#pragma once

#include "Scene.h"
#include "vendor/entt/include/entt.hpp"
#include "Scene/Components.h"

class Entity
{
private:
	entt::entity m_EntityHandle = { entt::null };
	Scene* m_Scene;

	friend Scene;

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
		if (!HasComponent<T>())
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

	operator bool() const { return m_EntityHandle != entt::null; }
	operator uint32_t() const { return (uint32_t)m_EntityHandle; }

	bool operator==(const Entity& other) const
	{
		return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
	}

	bool operator!=(const Entity& other) const
	{
		return !(*this == other);
	}
};