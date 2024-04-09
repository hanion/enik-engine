#pragma once

#include "scene/entity.h"
#include "scene/native_script_fields.h"

namespace Enik {

class ScriptableEntity {
public:
	ScriptableEntity() {}
	virtual ~ScriptableEntity() {}

	template <typename T>
	T& Get() {
		return m_Entity.Get<T>();
	}

	virtual std::vector<NativeScriptField> OnEditorGetFields() {
		return std::vector<NativeScriptField>{};
	}

protected:
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnUpdate(Timestep ts) {}
	virtual void OnFixedUpdate() {}

	virtual void OnCollision(Entity& other) {}

	virtual void OnKeyPressed (const KeyPressedEvent&  event) { }
	virtual void OnKeyReleased(const KeyReleasedEvent& event) { }
	virtual void OnMouseButtonPressed (const MouseButtonPressedEvent&  event) { }
	virtual void OnMouseButtonReleased(const MouseButtonReleasedEvent& event) { }
	virtual void OnMouseScrolled(const MouseScrolledEvent& event) { }

protected:
	std::string& GetTag() { return m_Entity.GetTag(); }

	bool HasFamily()                    { return m_Entity.HasFamily(); }
	Component::Family& GetOrAddFamily() { return m_Entity.GetOrAddFamily(); }
	bool HasParent()                    { return m_Entity.HasParent(); }
	Entity& GetParent()                 { return m_Entity.GetParent(); }
	void Reparent(Entity& new_parent)   {        m_Entity.Reparent(new_parent); }
	std::vector<Entity>& GetChildren()  { return m_Entity.GetChildren(); }

	Entity CreateEntity(const std::string& name = "") { return m_Entity.CreateEntity(name); }
	void DestroyEntity(Entity& entity) { m_Entity.DestroyEntity(entity); }

	Entity InstantiatePrefab(const std::filesystem::path& path) {
		return m_Entity.InstantiatePrefab(path);
	}

private:
	Entity m_Entity;
	friend class Scene;
	friend class PhysicsWorld;
};

}