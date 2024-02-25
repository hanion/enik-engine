#pragma once

#include "core/timestep.h"
#include "renderer/ortho_camera_controller.h"
#include "core/uuid.h"
#include <entt/entt.hpp>
#include "events/key_event.h"

namespace Enik {

class Entity;

class EN_API Scene {
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
	void DestroyEntity(Entity entity);

	entt::registry& Reg() { return m_Registry; }

	void OnUpdateEditor (Timestep ts, OrthographicCameraController& camera);
	void OnUpdateRuntime(Timestep ts);
	void OnFixedUpdate  ();
	void OnViewportResize(uint32_t width, uint32_t height);
	void OnViewportResize(glm::vec2 position, uint32_t width, uint32_t height);

	const std::string& GetName() const { return m_SceneName; }
	const void SetName(const std::string& name) { m_SceneName = name; }

	bool IsPaused() const { return m_IsPaused; }
	void SetPaused(bool is_paused) { m_IsPaused = is_paused; }

	void Step(int frames = 1) { m_StepFrames = frames; }

	void DestroyScriptableEntities();
	void ClearNativeScripts();

	Entity GetPrimaryCameraEntity();

	Entity FindEntityByUUID(UUID uuid);
	Entity FindEntityByName(const std::string& name);

private:
	entt::registry m_Registry;

	uint32_t m_ViewportWidth;
	uint32_t m_ViewportHeight;

	std::string m_SceneName = "untitled";

	bool m_IsPaused = false;
	int m_StepFrames = 0;

	friend class Entity;
	friend class SceneTreePanel;
	friend class InspectorPanel;
	friend class SceneSerializer;
};

}