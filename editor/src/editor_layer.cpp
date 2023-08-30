#include "editor_layer.h"

#include <pch.h>

#include <glm/gtc/matrix_transform.hpp>

#include "renderer/opengl/opengl_shader.h"
#include "scene/scene_serializer.h"


#define EDITOR_BIND_FUNC(fn) std::bind(&EditorLayer::fn, this)

EditorLayer::EditorLayer()
	: Layer("EditorLayer"), m_EditorCameraController(0,1280,0,600) {
}

void EditorLayer::OnAttach() {
	EN_PROFILE_SCOPE;

	FrameBufferSpecification spec;
	spec.Attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth};
	m_FrameBuffer = FrameBuffer::Create(spec);

	m_TexturePlay = Texture2D::Create(FULL_PATH_EDITOR("assets/icons/play_button.png"));
	m_TextureStop = Texture2D::Create(FULL_PATH_EDITOR("assets/icons/stop_button.png"));

	CreateNewScene();

	m_ToolbarPanel.InitValues(m_FrameBuffer, m_EditorCameraController, m_ViewportHovered);
	SetPanelsContext();
}

void EditorLayer::OnDetach() {
	EN_PROFILE_SCOPE;
}

void EditorLayer::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	m_Timestep = timestep;

	FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
	if (m_ViewportSize.x > 0.0f and m_ViewportSize.y > 0.0f and (spec.Width != m_ViewportSize.x or spec.Height != m_ViewportSize.y)) {
		m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_EditorCameraController.OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	Renderer2D::ResetStats();

	m_FrameBuffer->Bind();

	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	RenderCommand::Clear();
	m_FrameBuffer->ClearAttachment(1, -1);

	switch (m_SceneState) {
		case SceneState::Edit:
			m_ActiveScene->OnUpdateEditor(m_Timestep, m_EditorCameraController);
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(m_Timestep);
			break;
	}


	m_ToolbarPanel.OnUpdate();
	m_FrameBuffer->Unbind();

	m_EditorCameraController.OnUpdate(m_Timestep);
}

void EditorLayer::OnEvent(Event& event) {
	if (m_SceneState == SceneState::Edit and m_ViewportHovered) {
		m_EditorCameraController.OnEvent(event);
	}
	m_ToolbarPanel.OnEvent(event);

	EventDispatcher dispatcher = EventDispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(std::bind(&EditorLayer::OnKeyPressed, this, std::placeholders::_1));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(std::bind(&EditorLayer::OnMouseButtonReleased, this, std::placeholders::_1));
	dispatcher.Dispatch<MouseButtonPressedEvent> (std::bind(&EditorLayer::OnMouseButtonPressed, this, std::placeholders::_1));
}

void EditorLayer::OnImGuiRender() {
	/*DockSpace*/ {
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::Begin("DockSpace", nullptr, window_flags);

		ImGui::PopStyleVar(3);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("DockSpaceID");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New")) {
					DialogConfirm::OpenDialog("Create New Scene ?", EDITOR_BIND_FUNC(CreateNewScene));
				}

				if (ImGui::MenuItem("Open File")) {
					DialogFile::OpenDialog(DialogType::OPEN_FILE,
						[&](){
							LoadScene(DialogFile::GetSelectedPath());
						});
				}

				if (ImGui::MenuItem("Save")) {
					if (m_ActiveScenePath.empty()) {
						DialogFile::OpenDialog(DialogType::SAVE_FILE,
							[&](){
								m_ActiveScenePath = DialogFile::GetSelectedPath();
								SaveScene();
							});
					}
					else {
						SaveScene();
					}
				}
				if (ImGui::MenuItem("Save As")) {
					DialogFile::OpenDialog(DialogType::SAVE_FILE,
						[&](){
							m_ActiveScenePath = DialogFile::GetSelectedPath();
							SaveScene();
						});
				}

				if (ImGui::MenuItem("Exit")) {
					Application::Get().Close();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {
				ImGui::Checkbox("Show Performance", &m_ShowPerformance);
				ImGui::Checkbox("Show Renderer Stats", &m_ShowRendererStats);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		OnImGuiDockSpaceRender();

		ImGui::End();
	}

	DialogFile::Show();
	DialogConfirm::Show();
}

void EditorLayer::OnImGuiDockSpaceRender() {
	EN_PROFILE_SCOPE;

	m_SceneTreePanel.OnImGuiRender();
	m_InspectorPanel.OnImGuiRender();
	m_FileSystemPanel.OnImGuiRender();
	m_ToolbarPanel.OnImGuiRender(m_ViewportBounds[0], m_ViewportBounds[1]);

	/*Viewport*/ {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
		ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f), ImGuiCond_FirstUseEver);
		ImGui::Begin("Viewport", nullptr, window_flags);
		ImGui::PopStyleVar(1);

		auto viewport_min_region = ImGui::GetWindowContentRegionMin();
		auto viewport_max_region = ImGui::GetWindowContentRegionMax();
		auto viewport_offset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = {viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y};
		m_ViewportBounds[1] = {viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y};

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewport_size = ImGui::GetContentRegionAvail();
		m_ViewportSize = glm::vec2(viewport_size.x, viewport_size.y);
		if (((viewport_size.x != m_ViewportSize.x) or (viewport_size.y != m_ViewportSize.y)) and (viewport_size.x > 0 and viewport_size.y > 0)) {
			m_ViewportSize.x = viewport_size.x;
			m_ViewportSize.y = viewport_size.y;

			m_FrameBuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCameraController.OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		uint32_t texture_id = m_FrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(texture_id)),
					 ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

		/* Drag drop target */ {
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH", ImGuiDragDropFlags_AcceptBeforeDelivery)) {
					std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));

					if (std::filesystem::exists(path) and path.extension() == ".escn") {
						// draw rect to show it can be draggable
						ImVec2 drawStart = ImVec2(m_ViewportBounds[0].x + 2, m_ViewportBounds[0].y + 2);
						ImVec2 drawEnd   = ImVec2(m_ViewportBounds[1].x - 2, m_ViewportBounds[1].y - 2);
						ImGui::GetWindowDrawList()->AddRect(drawStart, drawEnd, IM_COL32(240, 240, 10, 240), 0.0f, ImDrawCornerFlags_All, 3.0f);

						if (payload->IsDelivery()) {
							LoadScene(path);
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::End();
	}

	/* ShowPerformance */
	if (m_ShowPerformance) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		window_flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking;

		ImGui::SetNextWindowBgAlpha(0.65f);
		ImVec2 pos;
		pos.x = 20 + m_ViewportBounds[0].x;
		pos.y = 40 + m_ViewportBounds[0].y;

		ImGui::SetNextWindowPos(pos);
		if (ImGui::Begin("Performance", nullptr, window_flags)) {
			ImGui::Text("Performance");
			ImGui::Text("deltaTime = %.2fms", m_Timestep.GetMilliseconds());
			ImGui::Text("FPS = %.0f", (1.0f / m_Timestep.GetSeconds()));
		}
		ImGui::End();
	}

	/* ShowRenderer2DStats */
	if (m_ShowRendererStats) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		window_flags |= ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking;

		ImGui::SetNextWindowBgAlpha(0.65f);
		ImVec2 pos;
		pos.x = 20 + m_ViewportBounds[0].x;
		pos.y = 130 + m_ViewportBounds[0].y;

		ImGui::SetNextWindowPos(pos);
		if (ImGui::Begin("Renderer2D Stats", nullptr, window_flags)) {
			auto stats = Renderer2D::GetStats();

			ImGui::Text("Renderer2D Stats");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quad Count: %d", stats.QuadCount);
			ImGui::Text("Total Vertex Count: %d", stats.GetTotalVertexCount());
			ImGui::Text("Total Index  Count: %d", stats.GetTotalIndexCount());
		}
		ImGui::End();
	}

	ShowToolbarPlayPause();
}

void EditorLayer::CreateNewScene() {
	m_EditorScene = CreateRef<Scene>();
	m_ActiveScene = m_EditorScene;
	SetPanelsContext();

	m_ActiveScenePath = std::filesystem::path();

}

void EditorLayer::LoadScene(const std::filesystem::path& path) {
	if (m_SceneState != SceneState::Edit) {
		// TODO popup a confirmation dialog
		OnSceneStop();
	}

	Ref<Scene> new_scene = CreateRef<Scene>();
	m_ActiveScenePath = path;
	SceneSerializer serializer = SceneSerializer(new_scene);
	if (serializer.Deserialize(m_ActiveScenePath)) {
		m_EditorScene = new_scene;
		m_ActiveScene = m_EditorScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		SetPanelsContext();
	}
}

void EditorLayer::SaveScene() {
	if (m_SceneState == SceneState::Edit) {
		// ? has no path, need to select
		if (m_ActiveScenePath.empty()) {
			DialogFile::OpenDialog(DialogType::SAVE_FILE,
				[&](){
					m_ActiveScenePath = DialogFile::GetSelectedPath();
					SaveScene();
				});
			return;
		}

		SceneSerializer serializer = SceneSerializer(m_ActiveScene);
		serializer.Serialize(m_ActiveScenePath);
	}
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& event) {
	if (event.IsRepeat()) {
		return false;
	}

	bool control = Input::IsKeyPressed(Key::LeftControl) or Input::IsKeyPressed(Key::RightControl);
	bool shift   = Input::IsKeyPressed(Key::LeftShift)   or Input::IsKeyPressed(Key::RightShift);

	switch (event.GetKeyCode()) {
		case Key::N:
			if (control) {
				DialogConfirm::OpenDialog("Create New Scene ?", EDITOR_BIND_FUNC(CreateNewScene));
			}
			break;

		case Key::O:
			if (control) {
				DialogFile::OpenDialog(DialogType::OPEN_FILE,
					[&](){
						LoadScene(DialogFile::GetSelectedPath());
					});
			}
			break;

		case Key::S:
			if (control and shift) {
				DialogFile::OpenDialog(DialogType::SAVE_FILE,
					[&](){
						m_ActiveScenePath = DialogFile::GetSelectedPath();
						SaveScene();
					});
			}
			else if (control) {
				SaveScene();
			}
			break;

		case Key::D:
			if (control) {
				SaveScene();
				SceneSerializer serializer = SceneSerializer(m_ActiveScene);

				auto& id = serializer.DuplicateEntity(m_ActiveScenePath, m_SceneTreePanel.GetSelectedEntity().Get<Component::ID>().uuid);
				m_SceneTreePanel.SetSelectedEntityWithUUID(id);

			}
			break;

		case Key::Delete:
			m_ActiveScene->DestroyEntity(m_SceneTreePanel.GetSelectedEntity());
			break;


		default:
			break;
	}

	return false;
}

bool EditorLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
	return false;
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
	if (event.GetMouseButton() == Mouse::ButtonLeft) {
		m_PickEntityWithMouse = true;
	}
	return false;
}


// ? play pause stop buttons
void EditorLayer::ShowToolbarPlayPause() {
	static const float toolbar_min_size = 32.0f;
	static const float padding = 4.0f;
	static float toolbar_window_width = 0.0f;
	ImVec2 pos;
	pos.x = m_ViewportBounds[1].x - toolbar_window_width - padding;
	pos.y = m_ViewportBounds[0].y + padding;
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowBgAlpha(0.65f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(toolbar_min_size, toolbar_min_size));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize;
	flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;

	if (not ImGui::Begin("##ToolbarPlayPause", nullptr, flags)) {
		ImGui::End();
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(3);
		return;
	}

	toolbar_window_width = ImGui::GetWindowSize().x;

	float size = ImGui::GetWindowHeight() - 4.0f;
	Ref<Texture2D> texture = (m_SceneState == SceneState::Edit) ? m_TexturePlay : m_TextureStop;
	auto texture_id = reinterpret_cast<void*>(static_cast<uintptr_t>(texture->GetRendererID()));
	if (ImGui::ImageButton(texture_id, ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), 0)) {
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

void EditorLayer::OnScenePlay() {
	SaveScene();

	if (m_ActiveScenePath.empty()) {
		return;
	}

	UUID current_selected_entity = m_SceneTreePanel.GetSelectedEntityUUID();

	m_SceneState = SceneState::Play;

	/* Copy Current Editor Scene */ {
		Ref<Scene> new_scene = CreateRef<Scene>();
		SceneSerializer serializer = SceneSerializer(new_scene);
		if (serializer.Deserialize(m_ActiveScenePath)) {
			m_ActiveScene = new_scene;
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			SetPanelsContext();
		}
	}

	m_SceneTreePanel.SetSelectedEntityWithUUID(current_selected_entity);

}

void EditorLayer::OnSceneStop() {
	UUID current_selected_entity = m_SceneTreePanel.GetSelectedEntityUUID();

	m_SceneState = SceneState::Edit;
	m_ActiveScene = m_EditorScene;
	SetPanelsContext();

	m_SceneTreePanel.SetSelectedEntityWithUUID(current_selected_entity);
}

void EditorLayer::SetPanelsContext() {
	m_SceneTreePanel.SetContext(m_ActiveScene);
	m_InspectorPanel.SetContext(m_ActiveScene, &m_SceneTreePanel);
	m_FileSystemPanel.SetContext(m_ActiveScene);
	m_ToolbarPanel.SetContext(m_ActiveScene, &m_SceneTreePanel);
}
