#pragma once

#include "scene/entity.h"
#include "scene/native_script_fields.h"
#include "physics/raycast.h"

namespace Enik {

class ScriptableEntity {
public:
	ScriptableEntity() {}
	virtual ~ScriptableEntity() {}

	template <typename T>
	T& Get() { return m_Entity.Get<T>(); }

	template <typename T>
	bool Has() const { return m_Entity.Has<T>(); }

	template <typename T>
	T& GetOrAdd() { return m_Entity.GetOrAdd<T>(); }


	const UUID& GetID() const { return m_Entity.GetID(); }
	std::string& GetTag() const { return m_Entity.GetTag(); }

	bool HasFamily() { return m_Entity.HasFamily(); }
	Component::Family& GetOrAddFamily() { return m_Entity.GetOrAddFamily(); }

	bool HasParent() { return m_Entity.HasParent(); }
	Entity GetParent() { return m_Entity.GetParent(); }

	void Reparent(Entity& new_parent) { m_Entity.Reparent(new_parent); }

	std::vector<Entity>& GetChildren() { return m_Entity.GetChildren(); }

	Entity CreateEntity(const std::string& name = "") { return m_Entity.CreateEntity(name); }
	void DestroyEntity(Entity entity) { m_Entity.DestroyEntity(entity); }

	Entity InstantiatePrefab(const std::filesystem::path& path) { return m_Entity.InstantiatePrefab(path); }

	Entity FindEntityByUUID(UUID uuid) { return m_Entity.FindEntityByUUID(uuid); }
	Entity FindEntityByName(const std::string& name) { return m_Entity.FindEntityByName(name); }

	void CloseApplication() { m_Entity.CloseApplication(); }


	void ChangeScene(const std::string& path) { m_Entity.ChangeScene(path); }

	Entity m_Entity;

public:
	virtual std::vector<NativeScriptField> OnEditorGetFields() {
		return std::vector<NativeScriptField>{};
	}

	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnUpdate(Timestep ts) {}
	virtual void OnFixedUpdate() {}

	virtual void OnCollisionEnter(Entity& other) {}
	virtual void OnCollisionExit (Entity& other) {}
	virtual void OnSensorEnter   (Entity& other) {}
	virtual void OnSensorExit    (Entity& other) {}

	virtual void OnKeyPressed (const KeyPressedEvent&  event) { }
	virtual void OnKeyReleased(const KeyReleasedEvent& event) { }
	virtual void OnMouseButtonPressed (const MouseButtonPressedEvent&  event) { }
	virtual void OnMouseButtonReleased(const MouseButtonReleasedEvent& event) { }
	virtual void OnMouseScrolled(const MouseScrolledEvent& event) { }

	RaycastResult CastRay(Raycast ray);

private:
	friend class Scene;
};

}