#pragma once
#include <Enik.h>

#include "dialogs/dialog_file.h"
#include "dialogs/dialog_confirm.h"
#include "panels/file_system.h"
#include "panels/inspector.h"
#include "panels/scene_tree.h"
#include "panels/toolbar.h"
#include "renderer/ortho_camera_controller.h"
#include "renderer/texture.h"


using namespace Enik;

class EditorLayer : public Layer {
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	virtual void OnAttach() override final;
	virtual void OnDetach() override final;

	virtual void OnUpdate(Timestep timestep) override final;
	virtual void OnFixedUpdate() override final;
	virtual void OnEvent(Event &event) override final;
	virtual void OnImGuiRender() override final;

	void OnImGuiDockSpaceRender();

private:
	void InitDockSpace();

	void CreateNewScene();
	void LoadScene(const std::filesystem::path &path);
	void SaveScene();

	void CreateNewProject();
	void LoadProject(const std::filesystem::path &path);
	void SaveProject();
	void ReloadProject();

	bool OnKeyPressed(KeyPressedEvent &event);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent &event);
	bool OnMouseButtonPressed (MouseButtonPressedEvent  &event);


	void ShowToolbarPlayPause();
	void OnScenePlay();
	void OnSceneStop();
	void OnScenePause(bool is_paused = false);

	void SetPanelsContext();

	void UpdateWindowTitle();

	void OnOverlayRender();

private:
	Ref<FrameBuffer> m_FrameBuffer;

	Ref<Scene> m_ActiveScene;
	Ref<Scene> m_EditorScene;

	OrthographicCameraController m_EditorCameraController;

	bool m_ViewportFocused = false;
	bool m_ViewportHovered = false;
	glm::vec2 m_ViewportSize = glm::vec2(0);
	glm::vec2 m_ViewportBounds[2];

	glm::vec2 m_ViewportPosition = glm::vec2(0);

	bool m_DockSpaceInitialized = false;

	bool m_ShowRendererStats = false;
	bool m_ShowPerformance   = false;
	bool m_ShowColliders     = true;

	Ref<Texture2D> m_TexturePlay, m_TextureStop, m_TexturePause, m_TextureStep;

	Timestep m_Timestep;

	SceneTreePanel m_SceneTreePanel;
	InspectorPanel m_InspectorPanel;
	FileSystemPanel m_FileSystemPanel;
	ToolbarPanel m_ToolbarPanel;

	std::filesystem::path m_ActiveScenePath = std::filesystem::canonical(".");

	enum class SceneState {
		Edit = 0,
		Play = 1
	};
	SceneState m_SceneState = SceneState::Edit;

	bool m_ShowSelectionOutline = true;
	glm::vec4 m_SelectionOutlineColor = glm::vec4(1.0f, 0.44f, 0.1f, 0.84f);
	int m_SelectionOutlineWidth = 5;

};