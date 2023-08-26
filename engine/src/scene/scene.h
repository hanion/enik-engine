#pragma once

#include "core/timestep.h"
#include "renderer/ortho_camera_controller.h"
#include "core/uuid.h"
#include <entt/entt.hpp>

namespace Enik {

class Entity;

class Scene {
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
	void DestroyEntity(const Entity& entity);

	entt::registry& Reg() { return m_Registry; }

	void OnUpdateEditor (Timestep ts, OrthographicCameraController& camera);
	void OnUpdateRuntime(Timestep ts);
	void OnViewportResize(uint32_t width, uint32_t height);

	const std::string& GetName() const { return m_SceneName; }
	const void SetName(const std::string& name) { m_SceneName = name; }


private:
	entt::registry m_Registry;

	uint32_t m_ViewportWidth;
	uint32_t m_ViewportHeight;

	std::string m_SceneName = "untitled";

	friend class Entity;
	friend class SceneTreePanel;
	friend class InspectorPanel;
	friend class SceneSerializer;
};

}