#pragma once

#include "core/timestep.h"
#include <entt/entt.hpp>

namespace Enik {

class Entity;

class Scene {
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());

	entt::registry& Reg() { return m_Registry; }

	void OnUpdate(Timestep ts);
	void OnViewportResize(uint32_t width, uint32_t height);


private:
	entt::registry m_Registry;

	uint32_t m_ViewportWidth;
	uint32_t m_ViewportHeight;

	friend class Entity;
	friend class SceneTreePanel;
};

}