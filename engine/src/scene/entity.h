#pragma once

#include "entt/entt.hpp"
#include "scene/scene.h"
#include "core/asserter.h"

namespace Enik {

class Entity {
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene);

	template<typename T, typename... Args>
	T& Add(Args&&... args){
		EN_CORE_ASSERT(!Has<T>(), "Entity already has component!");
		return m_Scene->m_Registry.emplace<T>(m_Hanle, std::forward<Args>(args)...);
	}
	
	template<typename T>
	T& Get() {
		EN_CORE_ASSERT(Has<T>(), "Entity does not have component!");
		return m_Scene->m_Registry.get<T>(m_Hanle);
	}

	
	template<typename T>
	bool Has() {
		return m_Scene->m_Registry.all_of<T>(m_Hanle);
	}

	template<typename T>
	void Revmove(T){
		EN_CORE_ASSERT(Has<T>(), "Entity does not have component!");
		return m_Scene->m_Registry.remove<T>(m_Hanle);
	}

	operator bool() const { return m_Hanle != entt::null; }

private:
	entt::entity m_Hanle{ entt::null };
	Scene* m_Scene = nullptr;

};


}