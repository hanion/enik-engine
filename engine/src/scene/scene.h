#pragma once

#include "core/timestep.h"
#include "renderer/ortho_camera_controller.h"
#include "core/uuid.h"
#include <entt/entt.hpp>
#include "events/key_event.h"

namespace Enik {

class Entity;

class Scene {
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
	void DestroyEntity(Entity entity);

	Entity InstantiatePrefab(const std::filesystem::path& path, UUID instance_uuid = UUID());

	entt::registry& Reg() { return m_Registry; }

	void OnUpdateEditor (Timestep ts, OrthographicCameraController& camera);
	void OnUpdateRuntime(Timestep ts);
	void OnFixedUpdate  ();
	void OnKeyPressed (const KeyPressedEvent&  event);
	void OnKeyReleased(const KeyReleasedEvent& event);
	void OnMouseButtonPressed (const MouseButtonPressedEvent&  event);
	void OnMouseButtonReleased(const MouseButtonReleasedEvent& event);
	void OnMouseScrolled(const MouseScrolledEvent& event);

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

	void SetGlobalTransforms();

	void CloseApplication();

	void ChangeScene(const std::string& path);

private:
	void ChangeToDeferredScene();

public:
	bool NeedViewportResize = false;

private:
	entt::registry m_Registry;

	uint32_t m_ViewportWidth;
	uint32_t m_ViewportHeight;

	std::string m_SceneName = "untitled";

	bool m_IsPaused = false;
	int m_StepFrames = 0;

	bool m_deferred_scene_change = false;
	std::string m_deferred_scene_path = "";

	friend class Entity;
	friend class SceneTreePanel;
	friend class InspectorPanel;
	friend class SceneSerializer;
	friend class PrefabEditorTab;
	friend class SceneEditorTab;
};

}