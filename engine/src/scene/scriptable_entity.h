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

private:
	Entity m_Entity;
	friend class Scene;
	friend class PhysicsWorld;
};

}