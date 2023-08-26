#pragma once

#include "scene/entity.h"

namespace Enik {

class ScriptableEntity {
public:
	virtual ~ScriptableEntity() {}

	template <typename T>
	T& Get() {
		return m_Entity.template Get<T>();
	}

protected:
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnUpdate(Timestep ts) {}

private:
	Entity m_Entity;
	friend class Scene;
};

}