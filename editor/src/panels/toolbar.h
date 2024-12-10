#pragma once
#include <Enik.h>
#include <base.h>

#include "renderer/ortho_camera_controller.h"
#include "scene_tree.h"

namespace Enik {

class ToolbarPanel {
public:
	ToolbarPanel() {}
	ToolbarPanel(const Ref<Scene>& context, SceneTreePanel* scene_tree_panel = nullptr);
	~ToolbarPanel() {}

	void SetContext(const Ref<Scene>& context, SceneTreePanel* scene_tree_panel = nullptr);

	void InitValues(const Ref<FrameBuffer>& frame_buffer, OrthographicCameraController& cam, bool& is_viewport_hovered);

	void OnUpdate();
	void OnImGuiRender(const glm::vec2& viewport_bound_min, const glm::vec2& viewport_bound_max);
	void OnEvent(Event& event);

private:
	void ShowToolbar();

	bool OnKeyPressed(KeyPressedEvent& event);
	bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);
	bool OnMouseMoved(MouseMovedEvent& event);

	void HandlePickEntityWithMouse();

	glm::vec2 GetMouseDelta();

	void Moving();
	void Scaling();
	void Rotating();

private:
	enum Tool {
		SELECT = 0,
		MOVE,
		SCALE,
		ROTATE
	};
	Tool m_SelectedTool = Tool::SELECT;
	bool ToolImageButton(const Ref<Texture2D>& texture, Tool tool);

private:
	Ref<Scene> m_Context;
	SceneTreePanel* m_SceneTreePanel;

	constexpr static float m_Padding = 4.0f;
	constexpr static float m_ToolbarMinSize = 32.0f;

	Ref<FrameBuffer> m_FrameBuffer;
	OrthographicCameraController* m_EditorCamera;

	glm::vec2 m_ViewportBoundMin, m_ViewportBoundMax;

	bool* m_ViewportHovered = nullptr;

	glm::vec2 m_MouseStart;
	bool m_ToolStart = false;
	bool m_ToolUsing = false;
};

}