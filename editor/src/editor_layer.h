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
	virtual void OnEvent(Event &event) override final;
	virtual void OnImGuiRender() override final;

	void OnImGuiDockSpaceRender();

private:
	void InitDockSpace();

	void CreateNewScene();
	void LoadScene(const std::filesystem::path &path);
	void SaveScene();

	bool OnKeyPressed(KeyPressedEvent &event);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent &event);
	bool OnMouseButtonPressed (MouseButtonPressedEvent  &event);


	void ShowToolbarPlayPause();
	void OnScenePlay();
	void OnSceneStop();
	void OnScenePause(bool is_paused = false);

	void SetPanelsContext();

private:
	Ref<FrameBuffer> m_FrameBuffer;

	Ref<Scene> m_ActiveScene;
	Ref<Scene> m_EditorScene;

	OrthographicCameraController m_EditorCameraController;

	bool m_ViewportFocused = false;
	bool m_ViewportHovered = false;
	glm::vec2 m_ViewportSize = glm::vec2(0);
	glm::vec2 m_ViewportBounds[2];

	bool m_DockSpaceInitialized = false;

	bool m_ShowRendererStats = false;
	bool m_ShowPerformance   = false;

	Entity m_Tile;
	Entity m_CameraEntity;

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
};