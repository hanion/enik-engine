#pragma once
#include <Enik.h>
#include "panels/scene_tree.h"
#include "panels/inspector.h"
#include "renderer/ortho_camera_controller.h"
#include "dialogs/dialog_file.h"


using namespace Enik;

class EditorLayer : public Layer {
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	virtual void OnAttach() override final;
	virtual void OnDetach() override final;

	virtual void OnUpdate(Timestep timestep) override final;
	virtual void OnEvent(Event& event) override final;
	virtual void OnImGuiRender() override final;

	void OnImGuiDockSpaceRender();

private:
	void CreateNewScene();
	void LoadScene(const std::string& path);
	void SaveScene();

	bool OnKeyPressed(KeyPressedEvent& event);

private:
	Ref<FrameBuffer> m_FrameBuffer;

	Ref<Scene> m_ActiveScene;

	OrthographicCameraController m_EditorCameraController;

	bool m_ViewportFocused = false;
	bool m_ViewportHovered = false;
	glm::vec2 m_ViewportSize = glm::vec2(0);
	glm::vec2 m_ViewportPosition = glm::vec2(0);

	bool m_ShowRendererStats = false;
	bool m_ShowPerformance   = true;

	Entity m_Tile;
	Entity m_CameraEntity;

	Timestep m_Timestep;

	SceneTreePanel m_SceneTreePanel;
	InspectorPanel m_InspectorPanel;

	std::string m_ActiveScenePath;

	DialogType m_ShowFileDialogAs = DialogType::OPEN_FILE;
	bool m_IsDialogOpen = false;
};