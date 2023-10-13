#pragma once

#include <entt/entt.hpp>
#include "scene/scene.h"
#include "core/asserter.h"

namespace Enik {

class Entity {
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene);

	template <typename T, typename... Args>
	T& Add(Args&&... args) {
		EN_CORE_ASSERT(!Has<T>(), "Entity already has component!");
		return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
	}

	template <typename T>
	T& Get() {
		EN_CORE_ASSERT(Has<T>(), "Entity does not have component!");
		return m_Scene->m_Registry.get<T>(m_Handle);
	}

	template <typename T>
	bool Has() {
		return m_Scene->m_Registry.all_of<T>(m_Handle);
	}

	template <typename T>
	void Remove() {
		EN_CORE_ASSERT(Has<T>(), "Entity does not have component!");
		m_Scene->m_Registry.remove<T>(m_Handle);
	}

	template <typename T>
	T& GetOrAdd() {
		if (Has<T>()) {
			return Get<T>();
		}
		return Add<T>();
	}

	operator bool() const { return m_Handle != entt::null; }

	operator entt::entity() const { return m_Handle; }
	operator uint32_t() const { return (uint32_t)m_Handle; }

	bool operator==(Entity other) {
		return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
	}
	bool operator!=(Entity other) {
		return !(*this == other);
	}

private:
	entt::entity m_Handle{entt::null};
	Scene* m_Scene = nullptr;
};

}