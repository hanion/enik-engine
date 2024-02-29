#pragma once

#include <entt/entt.hpp>
#include "scene/scene.h"
#include "core/asserter.h"
#include "scene/components.h"

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

	const UUID GetID() {
		return Get<Component::ID>().uuid;
	}

	std::string& GetTag() {
		return Get<Component::Tag>().Text;
	}

	ScriptableEntity* GetScriptInstance() {
		return Get<Component::NativeScript>().Instance;
	}



	bool HasFamily() {
		return Has<Component::Family>();
	}
	Component::Family& GetOrAddFamily() {
		return GetOrAdd<Component::Family>();
	}

	bool HasParent() {
		if (not HasFamily()) {
			return false;
		}
		Entity* parent = GetOrAddFamily().Parent;
		if (parent != nullptr) {
			return (*parent);
		}
		return false;
	}
	Entity& GetParent() {
		EN_CORE_ASSERT(HasParent(), "Entity does not have parent!");
		return *GetOrAddFamily().Parent;
	}
	void Reparent(Entity new_parent) {
		GetOrAddFamily().Reparent(*this, new_parent);
	}

	std::vector<Entity>& GetChildren() {
		return GetOrAddFamily().Children;
	}


	Entity CreateEntity(const std::string& name = std::string()) {
		return m_Scene->CreateEntity(name);
	}

	void DestroyEntity(Entity& entity) {
		m_Scene->DestroyEntity(entity);
	}

	Entity InstantiatePrefab(const std::filesystem::path& path) {
		return m_Scene->InstantiatePrefab(path);
	}


private:
	entt::entity m_Handle{entt::null};
	Scene* m_Scene = nullptr;
};

}