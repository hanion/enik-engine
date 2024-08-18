#include "toolbar.h"
#include "imgui.h"

namespace Enik {

ToolbarPanel::ToolbarPanel() {
	/* Create Textures */ {
		m_TextureSelect = Texture2D::Create(EN_ASSETS_PATH("icons/tool_select.png"));
		m_TextureMove   = Texture2D::Create(EN_ASSETS_PATH("icons/tool_move.png"));
		m_TextureScale  = Texture2D::Create(EN_ASSETS_PATH("icons/tool_scale.png"));
		m_TextureRotate = Texture2D::Create(EN_ASSETS_PATH("icons/tool_rotate.png"));
	}
}

ToolbarPanel::ToolbarPanel(const Ref<Scene>& context, SceneTreePanel* scene_tree_panel) {
	SetContext(context, scene_tree_panel);
}

void ToolbarPanel::SetContext(const Ref<Scene>& context, SceneTreePanel* scene_tree_panel) {
	m_Context = context;
	m_SceneTreePanel = scene_tree_panel;
}

void ToolbarPanel::InitValues(const Ref<FrameBuffer>& frame_buffer, OrthographicCameraController& cam, bool& is_viewport_hovered) {
	m_FrameBuffer = frame_buffer;
	m_EditorCamera = &cam;
	m_ViewportHovered = &is_viewport_hovered;
}

void ToolbarPanel::OnUpdate() {
	switch (m_SelectedTool) {
		case Tool::SELECT:
			HandlePickEntityWithMouse();
			break;
		case Tool::MOVE:
			Moving();
			break;
		case Tool::SCALE:
			Scaling();
			break;
		case Tool::ROTATE:
			Rotating();
			break;

		default:
			break;
	};
}

void ToolbarPanel::OnImGuiRender(const glm::vec2& viewport_bound_min, const glm::vec2& viewport_bound_max) {
	m_ViewportBoundMin = viewport_bound_min;
	m_ViewportBoundMax = viewport_bound_max;
	ShowToolbar();
}

void ToolbarPanel::ShowToolbar() {
	ImVec2 pos;
	pos.x = m_ViewportBoundMin.x + m_Padding;
	pos.y = m_ViewportBoundMin.y + m_Padding;
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowBgAlpha(0.65f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(m_ToolbarMinSize, m_ToolbarMinSize));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoDocking;

	if (not ImGui::Begin("##Toolbar", nullptr, flags)) {
		ImGui::End();
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(3);
		return;
	}

	if (ToolImageButton(m_TextureSelect, Tool::SELECT)) {
		m_SelectedTool = Tool::SELECT;
	}

	ImGui::SameLine();
	if (ToolImageButton(m_TextureMove, Tool::MOVE)) {
		m_SelectedTool = Tool::MOVE;
	}

	ImGui::SameLine();
	if (ToolImageButton(m_TextureRotate, Tool::ROTATE)) {
		m_SelectedTool = Tool::ROTATE;
	}

	ImGui::SameLine();
	if (ToolImageButton(m_TextureScale, Tool::SCALE)) {
		m_SelectedTool = Tool::SCALE;
	}

	ImGui::End();
	ImGui::PopStyleVar(4);
	ImGui::PopStyleColor(3);
}

bool ToolbarPanel::ToolImageButton(const Ref<Texture2D>& texture, Tool tool) {
	float size = m_ToolbarMinSize - m_Padding;
	auto texture_id = reinterpret_cast<void*>(static_cast<uintptr_t>(texture->GetRendererID()));

	ImVec4 tint_color = ImVec4(1, 1, 1, 1);
	if (m_SelectedTool == tool) {
		tint_color = ImVec4(0.6f, 0.6f, 0.9f, 1.0f);
	}

	return ImGui::ImageButton(texture_id, ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), 0, ImVec4(0, 0, 0, 0), tint_color);
}

void ToolbarPanel::OnEvent(Event& event) {
	EventDispatcher dispatcher = EventDispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(EN_BIND_EVENT_FN(ToolbarPanel::OnKeyPressed));
	dispatcher.Dispatch<MouseButtonPressedEvent>(EN_BIND_EVENT_FN(ToolbarPanel::OnMouseButtonPressed));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(EN_BIND_EVENT_FN(ToolbarPanel::OnMouseButtonReleased));
	dispatcher.Dispatch<MouseMovedEvent>(EN_BIND_EVENT_FN(ToolbarPanel::OnMouseMoved));
}

bool ToolbarPanel::OnKeyPressed(KeyPressedEvent& event) {
	if (event.IsRepeat()) {
		return false;
	}
	if (not *m_ViewportHovered) {
		return false;
	}

	bool control = Input::IsKeyPressed(Key::LeftControl) or Input::IsKeyPressed(Key::RightControl);
	bool shift   = Input::IsKeyPressed(Key::LeftShift)   or Input::IsKeyPressed(Key::RightShift);
	if (control or shift) {
		return false;
	}

	switch (event.GetKeyCode()) {
		case Key::Q:
			m_SelectedTool = Tool::SELECT;
			break;
		case Key::W:
			m_SelectedTool = Tool::MOVE;
			break;
		case Key::R:
			m_SelectedTool = Tool::SCALE;
			break;
		case Key::E:
			m_SelectedTool = Tool::ROTATE;
			break;
		default:
			return false;
	}

	event.Handled = true;
	m_ToolStart = false;
	m_ToolUsing = false;
	return false;
}

bool ToolbarPanel::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
	if (event.GetMouseButton() == Mouse::ButtonLeft) {
		m_ToolStart = *m_ViewportHovered;
		m_ToolUsing = *m_ViewportHovered;
	}

	return false;
}

bool ToolbarPanel::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
	if (event.GetMouseButton() == Mouse::ButtonLeft) {
		m_ToolStart = false;
		m_ToolUsing = false;
	}
	return false;
}

bool ToolbarPanel::OnMouseMoved(MouseMovedEvent& event) {
	if (m_ToolStart) {
		m_MouseStart = glm::vec2(event.GetX(), event.GetY());
		m_ToolStart = false;
	}

	return false;
}

void ToolbarPanel::HandlePickEntityWithMouse() {
	if (not m_ToolUsing) {
		return;
	}

	EN_PROFILE_SCOPE;

	ImVec2 mouse_pos = ImGui::GetMousePos();
	mouse_pos.x -= m_ViewportBoundMin.x;
	mouse_pos.y -= m_ViewportBoundMin.y;

	glm::vec2 viewport_size = m_ViewportBoundMax - m_ViewportBoundMin;

	mouse_pos.y = viewport_size.y - mouse_pos.y;

	int mouse_x = (int)mouse_pos.x;
	int mouse_y = (int)mouse_pos.y;

	if (mouse_x >= 0 and mouse_y >= 0 and mouse_x < (int)viewport_size.x and mouse_y < (int)viewport_size.y) {
		int pixel_data = m_FrameBuffer->ReadPixel(1, mouse_x, mouse_y);
		if (pixel_data == -1) {
			m_SceneTreePanel->SetSelectedEntity(Entity());
		}
		else {
			m_SceneTreePanel->SetSelectedEntity(Entity((entt::entity)pixel_data, m_Context.get()));
		}
	}

	m_ToolUsing = false;
}

glm::vec2 ToolbarPanel::GetMouseDelta() {
	auto [x, y] = Input::GetMousePosition();

	glm::vec2 current_pos = glm::vec2(x, y);

	glm::vec2 diff = current_pos - m_MouseStart;
	diff *= m_EditorCamera->GetZoomLevel() * 0.0035f;

	m_MouseStart = current_pos;

	return diff;
}

void ToolbarPanel::Moving() {
	if (not m_ToolUsing or m_ToolStart) {
		return;
	}

	if (m_SceneTreePanel->IsSelectedEntityValid()) {
		glm::vec2 diff = GetMouseDelta();

		glm::vec3& entity_pos = m_SceneTreePanel->GetSelectedEntity().Get<Component::Transform>().LocalPosition;
		entity_pos.x += diff.x;
		entity_pos.y -= diff.y;
	}
}

void ToolbarPanel::Scaling() {
	if (not m_ToolUsing or m_ToolStart) {
		return;
	}

	if (m_SceneTreePanel->IsSelectedEntityValid()) {
		glm::vec2 diff = GetMouseDelta();

		glm::vec3& entity_scale = m_SceneTreePanel->GetSelectedEntity().Get<Component::Transform>().LocalScale;
		entity_scale.x += diff.x;
		entity_scale.y += diff.y;
	}
}

void ToolbarPanel::Rotating() {
	if (not m_ToolUsing or m_ToolStart) {
		return;
	}

	if (m_SceneTreePanel->IsSelectedEntityValid()) {
		glm::vec2 diff = GetMouseDelta();
		// ? zoom level does not matter while rotating, intuitively
		diff *= 1.1f / m_EditorCamera->GetZoomLevel();

		m_SceneTreePanel->GetSelectedEntity().Get<Component::Transform>().LocalRotation += diff.x - diff.y;
	}
}

}