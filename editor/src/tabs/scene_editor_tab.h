#pragma once

#include "core/timestep.h"
#include "editor_tab.h"
#include "panels/animation_editor.h"
#include "renderer/texture.h"

#include "panels/file_system.h"
#include "panels/inspector.h"
#include "panels/scene_tree.h"
#include "panels/toolbar.h"
#include "panels/debug_info.h"
#include "renderer/ortho_camera_controller.h"

namespace Enik {

class SceneEditorTab : public EditorTab {
public:
	// TODO: name is the window name, this should also require an AssetHandle, which is the scene itself
	SceneEditorTab(const std::filesystem::path& name);
	virtual ~SceneEditorTab();

private:
	virtual void RenderContent() override final;
	virtual void InitializeDockspace() override final;

	virtual void OnUpdate(Timestep timestep) override final;
	virtual void OnFixedUpdate() override final;

	virtual void OnEvent(Event& event) override final;

	virtual void SetContext(EditorLayer* editor) override {
		m_EditorLayer = editor;
		SetPanelsContext();
	}

private:
	void SetPanelsContext();

	virtual void LoadScene(const std::filesystem::path& path);
	virtual void SaveScene();


	bool OnKeyPressed (KeyPressedEvent &event);
	bool OnKeyReleased(KeyReleasedEvent &event);
	bool OnMouseButtonPressed (MouseButtonPressedEvent  &event);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent &event);
	bool OnMouseScrolled(MouseScrolledEvent &event);

	void ShowToolbarPlayPause();
	void OnOverlayRender();

public:
	virtual void OnScenePlay();
	virtual void OnSceneStop();
	void OnScenePause(bool is_paused = false);


private:
	Timestep m_Timestep;

	Ref<FrameBuffer> m_FrameBuffer;

	Ref<Scene> m_ActiveScene;
	Ref<Scene> m_EditorScene;

	OrthographicCameraController m_EditorCameraController;

	const std::string m_ViewportPanelName;
	bool m_ViewportFocused = false;
	bool m_ViewportHovered = false;
	glm::vec2 m_ViewportSize = glm::vec2(0);
	glm::vec2 m_ViewportBounds[2];

	glm::vec2 m_ViewportPosition = glm::vec2(0);

	bool m_ShowColliders     = true;

	SceneTreePanel m_SceneTreePanel;
	InspectorPanel m_InspectorPanel;
	FileSystemPanel m_FileSystemPanel;
	ToolbarPanel m_ToolbarPanel;
	DebugInfoPanel m_DebugInfoPanel;
	AnimationEditorPanel m_AnimationEditorPanel;

	std::filesystem::path m_ActiveScenePath = std::filesystem::canonical(".");

	enum class SceneState {
		Edit = 0,
		Play = 1
	};
	SceneState m_SceneState = SceneState::Edit;

	bool m_ShowSelectionOutline = true;
	glm::vec4 m_SelectionOutlineColor = glm::vec4(1.0f, 0.44f, 0.1f, 0.84f);
	int m_SelectionOutlineWidth = 6;

	bool m_Appearing = true;

friend class PrefabEditorTab;
friend class FileSystemPanel;
};

}
