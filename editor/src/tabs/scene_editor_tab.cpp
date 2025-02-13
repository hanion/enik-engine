#include <pch.h>
#include "scene_editor_tab.h"
#include "asset/importer/texture_importer.h"
#include "events/event.h"
#include "events/mouse_event.h"
#include "imgui.h"
#include "scene/scene_serializer.h"
#include "dialogs/dialog_file.h"
#include "dialogs/dialog_confirm.h"

#include "editor_layer.h"
#include "script_system/extern_functions.h"
#include "utils/editor_assets.h"

#define BIND_FUNC_EVENT(fn) std::bind(&SceneEditorTab::fn, this, std::placeholders::_1)

namespace Enik {

SceneEditorTab::SceneEditorTab(const std::filesystem::path& name)
	: EditorTab(name), m_EditorCameraController(0,1280,0,600), m_ViewportPanelName("Viewport##" + std::to_string(m_TabID)) {

	FrameBufferSpecification spec;
	spec.Attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth};
	m_FrameBuffer = FrameBuffer::Create(spec);

	m_ToolbarPanel.InitValues(m_FrameBuffer, m_EditorCameraController, m_ViewportHovered);
	m_FileSystemPanel.SetCurrentDir(Project::GetProjectDirectory());

	std::filesystem::path path(name);
	SetWindowName(path.filename().string());

	LoadScene(Project::GetAbsolutePath(name));

	SetPanelsContext();
}

SceneEditorTab::~SceneEditorTab() {
	OnSceneStop();
	if (m_EditorScene != m_ActiveScene) {
		m_ActiveScene->ClearNativeScripts();
	}
}


void SceneEditorTab::SetPanelsContext() {
	m_SceneTreePanel.SetContext(m_ActiveScene);
	m_FileSystemPanel.SetContext(m_EditorLayer, this);
	m_ToolbarPanel.SetContext(m_ActiveScene, &m_SceneTreePanel);
	m_InspectorPanel.SetContext(m_ActiveScene, &m_SceneTreePanel, &m_AnimationEditorPanel, m_EditorLayer);
	m_Appearing = true;
}


void SceneEditorTab::OnUpdate(Timestep timestep) {
	m_Timestep = timestep;

	if (m_ActiveScene == nullptr) {
		return;
	}

	FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
	if (m_ViewportSize.x > 0.0f and m_ViewportSize.y > 0.0f and (spec.Width != m_ViewportSize.x or spec.Height != m_ViewportSize.y)) {
		m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_EditorCameraController.OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_ActiveScene->OnViewportResize(m_ViewportPosition, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	Renderer2D::ResetStats();

	m_FrameBuffer->Bind();

	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	RenderCommand::Clear();
	m_FrameBuffer->ClearAttachment(1, -1);

	switch (m_SceneState) {
		case SceneState::Edit:
			m_ActiveScene->OnUpdateEditor(timestep, m_EditorCameraController);
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(timestep);
			break;
	}

	OnOverlayRender();

	m_ToolbarPanel.OnUpdate();
	m_FrameBuffer->Unbind();

	m_EditorCameraController.OnUpdate(timestep);

	if (m_ActiveScene->NeedViewportResize) {
		m_ActiveScene->OnViewportResize(m_ViewportPosition, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}
}

void SceneEditorTab::OnFixedUpdate() {
	if (m_SceneState == SceneState::Play) {
		m_ActiveScene->OnFixedUpdate();
	}
}

void SceneEditorTab::OnEvent(Event& event) {
	if (m_SceneState == SceneState::Edit) {
		m_EditorCameraController.OnEvent(event, m_ViewportHovered);
	}
	if (m_SceneState == SceneState::Edit or m_ActiveScene->IsPaused()) {
		m_ToolbarPanel.OnEvent(event);
	}

	EventDispatcher dispatcher = EventDispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>         (BIND_FUNC_EVENT(OnKeyPressed));
	dispatcher.Dispatch<KeyReleasedEvent>        (BIND_FUNC_EVENT(OnKeyReleased));
	dispatcher.Dispatch<MouseButtonPressedEvent> (BIND_FUNC_EVENT(OnMouseButtonPressed));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_FUNC_EVENT(OnMouseButtonReleased));
	dispatcher.Dispatch<MouseScrolledEvent>      (BIND_FUNC_EVENT(OnMouseScrolled));
}

void SceneEditorTab::RenderContent() {
	// tint editor while playing
	int pushed_style_color_count = 0;
	if (m_SceneState == SceneState::Play) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::dim);
		pushed_style_color_count++;
	}

	if (m_ActiveScene == nullptr) {
		return;
	}


	m_SceneTreePanel.OnImGuiRender();
	m_InspectorPanel.OnImGuiRender();
	m_AnimationEditorPanel.OnImGuiRender();
	if (m_Appearing) { ImGui::SetNextWindowFocus(); }
	m_FileSystemPanel.OnImGuiRender();

	bool is_viewport_open = false;

	/*Viewport*/ {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::PanelTabPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
		ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f), ImGuiCond_FirstUseEver);
		is_viewport_open = ImGui::Begin(m_ViewportPanelName.c_str(), nullptr, window_flags);
		

		auto viewport_min_region = ImGui::GetWindowContentRegionMin();
		auto viewport_max_region = ImGui::GetWindowContentRegionMax();
		auto viewport_offset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = {viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y};
		m_ViewportBounds[1] = {viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y};

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		m_ViewportPosition.x = m_ViewportBounds[0].x - viewport->Pos.x;
		m_ViewportPosition.y = m_ViewportBounds[0].y - viewport->Pos.y;

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewport_size = ImGui::GetContentRegionAvail();
		m_ViewportSize = glm::vec2(viewport_size.x, viewport_size.y);

		if (is_viewport_open) {
			uint32_t texture_id = m_FrameBuffer->GetColorAttachmentRendererID();
			ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(texture_id)),
						ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
		}

		/* Drag drop target */ {
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH", ImGuiDragDropFlags_AcceptBeforeDelivery)) {
					std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
					path = Project::GetAbsolutePath(path);

					if (std::filesystem::exists(path) and (path.extension() == ".escn" or path.extension() == ".prefab")) {
						// draw rect to show it can be draggable
						ImVec2 drawStart = ImVec2(m_ViewportBounds[0].x + 2, m_ViewportBounds[0].y + 2);
						ImVec2 drawEnd   = ImVec2(m_ViewportBounds[1].x - 2, m_ViewportBounds[1].y - 2);
						ImGui::GetWindowDrawList()->AddRect(drawStart, drawEnd, IM_COL32(240, 240, 10, 240), 0.0f, ImDrawCornerFlags_All, 3.0f);

						if (payload->IsDelivery()) {
							if (path.extension() == ".escn") {
								if (m_EditorLayer) {
									m_EditorLayer->RequestOpenAsset(Project::GetRelativePath(path));
								}
							}
							else if (path.extension() == ".prefab") {
								Entity prefab = m_ActiveScene->InstantiatePrefab(path.string());
								m_SceneTreePanel.SetSelectedEntity(prefab);
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}


	if (is_viewport_open) {
		ShowToolbarPlayPause();
		if (m_SceneState == SceneState::Edit or m_ActiveScene->IsPaused()) {
			m_ToolbarPanel.OnImGuiRender(m_ViewportBounds[0], m_ViewportBounds[1]);
		}
	}
	if (is_viewport_open) {
		ImGui::SetNextWindowPos({ m_ViewportBounds[0].x + 10, m_ViewportBounds[0].y + 40 });
		glm::vec2 viewport_size = {
			m_ViewportBounds[1].x - m_ViewportBounds[0].x,
			m_ViewportBounds[1].y - m_ViewportBounds[0].y
		};
		m_DebugInfoPanel.ShowDebugInfoPanel(m_Timestep, viewport_size);
	}


		ImGui::End();
	}

	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(pushed_style_color_count);
	m_Appearing = false;
}

void SceneEditorTab::InitializeDockspace() {
	ImGuiID dock_id_1 = ImGui::DockBuilderSplitNode(m_DockspaceID, ImGuiDir_Left,  0.42f, NULL, &m_DockspaceID);
	ImGuiID dock_id_2 = ImGui::DockBuilderSplitNode(dock_id_1,     ImGuiDir_Right, 0.52f, NULL, &dock_id_1);
// 	ImGuiID dock_id_3 = ImGui::DockBuilderSplitNode(dock_id_1,     ImGuiDir_Down,  0.40f, NULL, &dock_id_1);
	ImGuiID dock_id_4 = ImGui::DockBuilderSplitNode(m_DockspaceID, ImGuiDir_Down,  0.32f, NULL, &m_DockspaceID);

	ImGui::DockBuilderDockWindow(m_ViewportPanelName.c_str(), m_DockspaceID);
	m_SceneTreePanel.DockTo(dock_id_1);
	m_InspectorPanel.DockTo(dock_id_2);
	m_AnimationEditorPanel.DockTo(dock_id_4);
	m_FileSystemPanel.DockTo(dock_id_4);
	m_Appearing = true;
}


void SceneEditorTab::LoadScene(const std::filesystem::path& path) {
	if (not std::filesystem::exists(path)) {
		return;
	}
	if (path.has_extension() and path.extension() != ".escn") {
		return;
	}

	if (m_SceneState != SceneState::Edit) {
		OnSceneStop();
	}

	Ref<Scene> new_scene = CreateRef<Scene>();
	SceneSerializer serializer = SceneSerializer(new_scene);
	if (serializer.Deserialize(path.string())) {
		m_EditorScene = new_scene;
		m_ActiveScene = m_EditorScene;
		m_ActiveScenePath = path;
		m_ActiveScene->OnViewportResize(m_ViewportPosition, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		SetPanelsContext();
	}
	
}

void SceneEditorTab::SaveScene() {
	if (m_SceneState == SceneState::Edit) {
		// ? has no path, need to select
		if (m_ActiveScenePath.empty()) {
			DialogFile::OpenDialog(DialogType::SAVE_FILE,
				[&](){
					m_ActiveScenePath = DialogFile::GetSelectedPath();
					SaveScene();
				}, ".escn");
			return;
		}

		SceneSerializer serializer = SceneSerializer(m_ActiveScene);
		serializer.Serialize(m_ActiveScenePath.string());
		Project::GetAssetManagerEditor()->SerializeAssetRegistry();
	}
}




bool SceneEditorTab::OnKeyPressed(KeyPressedEvent& event) {
	if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnKeyPressed(event);
	}

	if (event.IsRepeat()) {
		return false;
	}

	bool control = Input::IsKeyPressed(Key::LeftControl) or Input::IsKeyPressed(Key::RightControl);
	bool shift   = Input::IsKeyPressed(Key::LeftShift)   or Input::IsKeyPressed(Key::RightShift);

	switch (event.GetKeyCode()) {
		case Key::S:
			if (control and shift) {
				DialogFile::OpenDialog(DialogType::SAVE_FILE,
					[&](){
						m_ActiveScenePath = DialogFile::GetSelectedPath();
						SaveScene();
					}, ".escn");
			}
			else if (control) {
				SaveScene();
			}
			break;
		case Key::D:
			if (control and m_SceneTreePanel.IsSelectedEntityValid()) {
				SaveScene();
				SceneSerializer serializer = SceneSerializer(m_ActiveScene);

				auto& id = m_SceneTreePanel.GetSelectedEntity().Get<Component::ID>().uuid;
				auto& new_id = serializer.DuplicateEntity(m_ActiveScenePath.string(), id);
				m_SceneTreePanel.SetSelectedEntityWithUUID(new_id);

			}
			break;

		case Key::Delete:
			if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0) {
				if (m_SceneTreePanel.IsSelectedEntityValid()) {
					DialogConfirm::OpenDialog(
						"Delete Entity ?",
						m_SceneTreePanel.GetSelectedEntity().GetTag(),
						[&](){
							m_ActiveScene->DestroyEntity(m_SceneTreePanel.GetSelectedEntity());
						}
					);
				}
			}
			break;

		case Key::F5:
			if (m_SceneState == SceneState::Edit) {
				OnScenePlay();
			}
			else if (m_SceneState == SceneState::Play) {
				OnSceneStop();
			}
			break;
		case Key::F7:
			OnScenePause(not m_ActiveScene->IsPaused());
			break;
		case Key::F8:
			if (m_SceneState == SceneState::Play) {
				m_ActiveScene->Step();
			}
			break;
	}
	return false;
}


bool SceneEditorTab::OnKeyReleased(KeyReleasedEvent& event) {
    if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnKeyReleased(event);
	}
	m_DebugInfoPanel.OnKeyReleased(event);
	return false;
}

bool SceneEditorTab::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
	m_SceneTreePanel.OnMouseButtonReleased(event);

	if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnMouseButtonReleased(event);
	}
	return false;
}

bool SceneEditorTab::OnMouseScrolled(MouseScrolledEvent& event) {
    if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnMouseScrolled(event);
	}
	return false;
}

bool SceneEditorTab::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
	if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnMouseButtonPressed(event);
	}
	return false;
}















// ? play pause stop buttons
void SceneEditorTab::ShowToolbarPlayPause() {
	constexpr ImVec2 toolbar_size = ImVec2(32.0f, 32.0f);
	const float padding = 4.0f;

	int visible_button_count = 1;
	if (m_SceneState == SceneState::Play) {
		visible_button_count++;
	}
	if (m_ActiveScene->IsPaused()) {
		visible_button_count++;
	}


	ImVec2 pos;
	pos.x = m_ViewportBounds[1].x - toolbar_size.x * visible_button_count - padding - padding;
	pos.y = m_ViewportBounds[0].y + padding;
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowBgAlpha(0.65f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, toolbar_size);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavInputs;

	if (not ImGui::Begin("##ToolbarPlayPause", nullptr, flags)) {
		ImGui::End();
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(3);
		return;
	}

	if (m_SceneState == SceneState::Play) {
		if (m_ActiveScene->IsPaused()) {
			auto texture_id_step = reinterpret_cast<void*>(static_cast<uintptr_t>(EditorAssets::Step->GetRendererID()));
			if (ImGui::ImageButton(texture_id_step, toolbar_size, ImVec2(0, 1), ImVec2(1, 0), 0)) {
				m_ActiveScene->Step();
			}
			ImGui::SameLine();
		}

		ImVec4 tint_color = ImVec4(1, 1, 1, 1);
		if (m_ActiveScene->IsPaused()) {
			tint_color = ImVec4(0.5f, 0.5f, 0.9f, 1.0f);
		}

		auto texture_id_pause = reinterpret_cast<void*>(static_cast<uintptr_t>(EditorAssets::Pause->GetRendererID()));
		if (ImGui::ImageButton(texture_id_pause, toolbar_size, ImVec2(0, 1), ImVec2(1, 0), 0, ImVec4(0, 0, 0, 0), tint_color)) {
			OnScenePause(not m_ActiveScene->IsPaused());
		}
		ImGui::SameLine();
	}



	Ref<Texture2D> texture = (m_SceneState == SceneState::Edit) ? EditorAssets::Play : EditorAssets::Stop;
	auto texture_id = reinterpret_cast<void*>(static_cast<uintptr_t>(texture->GetRendererID()));
	if (ImGui::ImageButton(texture_id, toolbar_size, ImVec2(0, 1), ImVec2(1, 0), 0)) {
		if (m_SceneState == SceneState::Edit) {
			OnScenePlay();
		}
		else if (m_SceneState == SceneState::Play) {
			OnSceneStop();
		}
	}


	ImGui::End();
	ImGui::PopStyleVar(4);
	ImGui::PopStyleColor(3);
}

void SceneEditorTab::OnScenePlay() {
	OnScenePause(false);
	// SaveScene();

	if (m_ActiveScenePath.empty()) {
		return;
	}

	Enik::UUID current_selected_entity = m_SceneTreePanel.GetSelectedEntityUUID();

	m_SceneState = SceneState::Play;

	/* Copy Current Editor Scene */ {
		Ref<Scene> new_scene = CreateRef<Scene>();
		SceneSerializer serializer = SceneSerializer(new_scene);
		if (serializer.Deserialize(m_ActiveScenePath.string())) {
			m_ActiveScene = new_scene;
			m_ActiveScene->OnViewportResize(m_ViewportPosition, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			SetPanelsContext();
		}
	}

	m_SceneTreePanel.SetSelectedEntityWithUUID(current_selected_entity);
	ImGui::SetWindowFocus(m_ViewportPanelName.c_str());

}

void SceneEditorTab::OnSceneStop() {
	OnScenePause(false);

	Tween::ResetData();

	Enik::UUID current_selected_entity = 0;
	if (m_SceneTreePanel.IsSelectedEntityValid()) {
		current_selected_entity = m_SceneTreePanel.GetSelectedEntityUUID();
	}

	m_SceneState = SceneState::Edit;
	m_ActiveScene = m_EditorScene;
	SetPanelsContext();

	m_SceneTreePanel.SetSelectedEntityWithUUID(current_selected_entity);
}

void SceneEditorTab::OnScenePause(bool is_paused) {
	if (m_SceneState == SceneState::Edit) {
		return;
	}
	m_ActiveScene->SetPaused(is_paused);
}














void SceneEditorTab::OnOverlayRender() {
	if (not (m_SceneState == SceneState::Edit or m_ActiveScene->IsPaused())) {
		return;
	}

	if (m_SceneState == SceneState::Edit) {
		Renderer2D::BeginScene(m_EditorCameraController.GetCamera());
	}
	else {
		Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
		if (not camera) {
			return;
		}

		Renderer2D::BeginScene(
			camera.Get<Component::Camera>().Cam,
			camera.Get<Component::Transform>().GetTransform()
		);
	}

	if (m_ShowColliders) {
		const float line_thickness = std::clamp(m_EditorCameraController.GetZoomLevel()*0.01f, 0.01f, 0.3f);
		auto view = m_ActiveScene->Reg().view<Component::CollisionShape, Component::Transform>();
		for(auto& entity : view) {
			auto [transform, collider] = view.get<Component::Transform, Component::CollisionShape>(entity);
			switch (collider.Shape) {
				case Component::CollisionShape::Type::CIRCLE: {
					Renderer2D::DrawCircle(
						glm::vec3(
							transform.GlobalPosition.x + collider.CircleCenter.x,
							transform.GlobalPosition.y + collider.CircleCenter.y,
							0.998f
						),
						transform.LocalScale.x * collider.CircleRadius,
						32,
						glm::vec4(0.3f, 0.8f, 0.3f, 1.0f),
						line_thickness
					);
					break;
				}
				case Component::CollisionShape::Type::BOX: {
					Component::Transform trans;
					trans.GlobalPosition = glm::vec3(
						transform.GlobalPosition.x,
						transform.GlobalPosition.y,
						0.998f
					);
					trans.GlobalRotation = transform.GlobalRotation;
					trans.GlobalScale = transform.GlobalScale * collider.BoxScale * 2.0f;
					Renderer2D::DrawRect(trans, glm::vec4(0.3f, 0.8f, 0.3f, 1.0f), line_thickness);
					break;
				}
				case Component::CollisionShape::Type::NONE: break;

			}

		}

	}

	if (m_ShowSelectionOutline) {
		if (m_SceneTreePanel.IsSelectedEntityValid()) {
			Entity selected = m_SceneTreePanel.GetSelectedEntity();
			Component::Transform transform = selected.Get<Component::Transform>();
			transform.GlobalPosition.z = 0.999f;

			if (selected.Has<Component::Text>()) {
				glm::vec2 bb = selected.Get<Component::Text>().GetBoundingBox();
				bb = glm::max(glm::vec2{1}, bb);

				transform.GlobalScale.y = bb.y;
				transform.GlobalPosition.y -= bb.y * 0.5f;

				transform.GlobalScale.x = -bb.x;
				transform.GlobalPosition.x += bb.x * 0.5f;
			}

			Renderer2D::DrawRect(transform, m_SelectionOutlineColor, m_EditorCameraController.GetZoomLevel()*0.018f);
		}
	}


	Renderer2D::EndScene();
}



}
