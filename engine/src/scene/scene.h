#pragma once

#include "core/timestep.h"
#include "entt/entt.hpp"

namespace Enik {

class Entity;

class Scene {
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());

	entt::registry& Reg() { return m_Registry; }

	void OnUpdate(Timestep ts);

private:
	entt::registry m_Registry;

	friend class Entity;
};

}