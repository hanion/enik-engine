#include "editor_layer.h"

#include <pch.h>

#include <glm/gtc/matrix_transform.hpp>

#include "renderer/opengl/opengl_shader.h"
#include "scene/scene_serializer.h"
#include <imgui/imgui_internal.h>
#include "utils/editor_colors.h"



#define BIND_FUNC(fn) std::bind(&EditorLayer::fn, this)
#define BIND_FUNC_EVENT(fn) std::bind(&EditorLayer::fn, this, std::placeholders::_1)

EditorLayer::EditorLayer()
	: Layer("EditorLayer"), m_EditorCameraController(0,1280,0,600) {
}

void EditorLayer::OnAttach() {
	EN_PROFILE_SCOPE;

	FrameBufferSpecification spec;
	spec.Attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth};
	m_FrameBuffer = FrameBuffer::Create(spec);

	m_TexturePlay  = Texture2D::Create(FULL_PATH_EDITOR("assets/icons/play_button.png"));
	m_TextureStop  = Texture2D::Create(FULL_PATH_EDITOR("assets/icons/stop_button.png"));
	m_TexturePause = Texture2D::Create(FULL_PATH_EDITOR("assets/icons/pause_button.png"));
	m_TextureStep  = Texture2D::Create(FULL_PATH_EDITOR("assets/icons/step_button.png"));

	// TODO prompt user to select a name and a directory
	// CreateNewProject();
	std::filesystem::path proj(FULL_PATH_EDITOR("project.enik"));
	LoadProject(proj);


	m_ToolbarPanel.InitValues(m_FrameBuffer, m_EditorCameraController, m_ViewportHovered);
	SetPanelsContext();
}

void EditorLayer::OnDetach() {
	EN_PROFILE_SCOPE;

	if (m_EditorScene != m_ActiveScene) {
		m_ActiveScene->ClearNativeScripts();
	}
	ScriptSystem::UnloadScriptModule();
}

void EditorLayer::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	if (m_ActiveScene == nullptr) {
		return;
	}

	m_Timestep = timestep;

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
			m_ActiveScene->OnUpdateEditor(m_Timestep, m_EditorCameraController);
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(m_Timestep);
			break;
	}

	OnOverlayRender();

	m_ToolbarPanel.OnUpdate();
	m_FrameBuffer->Unbind();

	m_EditorCameraController.OnUpdate(m_Timestep);
}

void EditorLayer::OnFixedUpdate() {
	if (m_SceneState == SceneState::Play) {
		m_ActiveScene->OnFixedUpdate();
	}
}

void EditorLayer::OnEvent(Event& event) {
	if (m_SceneState == SceneState::Edit) {
		m_EditorCameraController.OnEvent(event, m_ViewportHovered);
	}
	if (m_SceneState == SceneState::Edit or m_ActiveScene->IsPaused()) {
		m_ToolbarPanel.OnEvent(event);
	}

	EventDispatcher dispatcher = EventDispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>         (BIND_FUNC_EVENT(OnKeyPressed));
	dispatcher.Dispatch<KeyReleasedEvent>        (BIND_FUNC_EVENT(OnKeyReleased));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_FUNC_EVENT(OnMouseButtonReleased));
	dispatcher.Dispatch<MouseButtonPressedEvent> (BIND_FUNC_EVENT(OnMouseButtonPressed));
	dispatcher.Dispatch<MouseScrolledEvent>      (BIND_FUNC_EVENT(OnMouseScrolled));
}

void EditorLayer::OnImGuiRender() {
	// tint editor while playing
	int pushed_style_color_count = 0;
	if (m_SceneState == SceneState::Play) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::dim);
		pushed_style_color_count++;
	}

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

				if (ImGui::BeginMenu("Project")) {
					if (ImGui::MenuItem("Reload Project")) {
						ReloadProject();
					}
					if (ImGui::MenuItem("Open Project")) {
						DialogFile::OpenDialog(DialogType::OPEN_FILE,
							[&](){
								LoadProject(DialogFile::GetSelectedPath());
							}, ".enik");
					}
					if (ImGui::MenuItem("Save Project")) {
						SaveProject();
					}
					ImGui::EndMenu();
				}

				if (m_ActiveScene != nullptr and ImGui::BeginMenu("Scene")) {
					if (ImGui::MenuItem("New Scene")) {
						DialogConfirm::OpenDialog("Create New Scene ?", BIND_FUNC(CreateNewScene));
					}
					if (ImGui::MenuItem("Open Scene")) {
						DialogFile::OpenDialog(DialogType::OPEN_FILE,
							[&](){
								LoadScene(DialogFile::GetSelectedPath());
							}, ".escn");
					}

					if (ImGui::MenuItem("Save Scene")) {
						if (m_ActiveScenePath.empty()) {
							DialogFile::OpenDialog(DialogType::SAVE_FILE,
								[&](){
									m_ActiveScenePath = DialogFile::GetSelectedPath();
									SaveScene();
								}, ".escn");
						}
						else {
							SaveScene();
						}
					}
					if (ImGui::MenuItem("Save Scene As")) {
						DialogFile::OpenDialog(DialogType::SAVE_FILE,
							[&](){
								m_ActiveScenePath = DialogFile::GetSelectedPath();
								SaveScene();
							}, ".escn");
					}
					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Exit")) {
					Application::Get().Close();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {
				ImGui::Checkbox("Show Performance", &m_ShowPerformance);
				ImGui::Checkbox("Show Renderer Stats", &m_ShowRendererStats);
				ImGui::Checkbox("Show Colliders", &m_ShowColliders);
				ImGui::Checkbox("Show Selection Outline", &m_ShowSelectionOutline);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		OnImGuiDockSpaceRender();

		ImGui::End();
	}

	DialogFile::Show();
	DialogConfirm::Show();

	ImGui::PopStyleColor(pushed_style_color_count);
}

void EditorLayer::OnImGuiDockSpaceRender() {
	EN_PROFILE_SCOPE;

	if (m_ActiveScene == nullptr) {
		return;
	}

	m_SceneTreePanel.OnImGuiRender();
	m_InspectorPanel.OnImGuiRender();
	m_FileSystemPanel.OnImGuiRender();
	}

	bool is_viewport_open = false;

	/*Viewport*/ {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
		ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f), ImGuiCond_FirstUseEver);
		is_viewport_open = ImGui::Begin("Viewport", nullptr, window_flags);
		ImGui::PopStyleVar(1);
		if (is_viewport_open) {
			ShowToolbarPlayPause();
			if (m_SceneState == SceneState::Edit or m_ActiveScene->IsPaused()) {
				m_ToolbarPanel.OnImGuiRender(m_ViewportBounds[0], m_ViewportBounds[1]);
			}
		}

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
		if (((viewport_size.x != m_ViewportSize.x) or (viewport_size.y != m_ViewportSize.y)) and (viewport_size.x > 0 and viewport_size.y > 0)) {
			m_ViewportSize.x = viewport_size.x;
			m_ViewportSize.y = viewport_size.y;

			m_FrameBuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize(m_ViewportPosition, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCameraController.OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

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
								LoadScene(path);
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

		ImGui::End();
	}

	/* ShowPerformance */
	if (m_ShowPerformance and is_viewport_open) {
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
	if (m_ShowRendererStats and is_viewport_open) {
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

	InitDockSpace();
}

void EditorLayer::InitDockSpace() {
	if (m_DockSpaceInitialized) {
		return;
	}

	ImGuiID dockspace_id = ImGui::GetID("DockSpaceID");
	ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
	ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
	ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

	ImGuiID dock_id_main = dockspace_id;
	ImGuiID dock_id_1 = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Left,  0.42f, NULL, &dock_id_main);
	ImGuiID dock_id_2 = ImGui::DockBuilderSplitNode(dock_id_1,    ImGuiDir_Right, 0.52f, NULL, &dock_id_1);
	ImGuiID dock_id_3 = ImGui::DockBuilderSplitNode(dock_id_1,    ImGuiDir_Down,  0.45f, NULL, &dock_id_1);

	ImGui::DockBuilderDockWindow("Viewport",    dock_id_main);
	ImGui::DockBuilderDockWindow("Scene Tree",  dock_id_1);
	ImGui::DockBuilderDockWindow("Inspector",   dock_id_2);
	ImGui::DockBuilderDockWindow("File System", dock_id_3);
	ImGui::DockBuilderFinish(dockspace_id);

	m_DockSpaceInitialized = true;
}


void EditorLayer::CreateNewScene() {
	m_EditorScene = CreateRef<Scene>();
	m_ActiveScene = m_EditorScene;
	SetPanelsContext();

	m_ActiveScenePath = std::filesystem::path();

	UpdateWindowTitle();
}

void EditorLayer::LoadScene(const std::filesystem::path& path) {
	if (m_SceneState != SceneState::Edit) {
		// TODO popup a confirmation dialog
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
		UpdateWindowTitle();
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
				}, ".escn");
			return;
		}

		SceneSerializer serializer = SceneSerializer(m_ActiveScene);
		serializer.Serialize(m_ActiveScenePath.string());
		UpdateWindowTitle();
	}
}

void EditorLayer::CreateNewProject() {
	Project::New();
	DialogFile::OpenDialog(DialogType::SAVE_FILE,
		[&](){
			// TODO this
			EN_CORE_WARN("TODO Create a new project here. (EditorLayer::CreateNewProject)");
			std::filesystem::path proj(FULL_PATH_EDITOR("project.enik"));
			LoadProject(proj);

		}, ".enik");
}

void EditorLayer::LoadProject(const std::filesystem::path& path) {
	if (Project::Load(path)) {
		auto start_scene_path = Project::GetAbsolutePath(Project::GetActive()->GetConfig().start_scene);
		ScriptRegistry::ClearRegistry();
		ScriptSystem::LoadScriptModuleFirstTime();
		LoadScene(start_scene_path);
		m_FileSystemPanel.SetCurrentDir(Project::GetProjectDirectory());
		DialogFile::SetCurrentDir(Project::GetProjectDirectory());

		ScriptSystem::ClearOnScriptModuleReloadEvents();
		ScriptSystem::CallOnScriptModuleReload(
			[&]() {
				SceneSerializer serializer = SceneSerializer(m_ActiveScene);
				serializer.ReloadNativeScriptFields(m_ActiveScenePath.string());
			}
		);
	}
}

void EditorLayer::SaveProject() {
	Project::Save(Project::GetActive()->GetProjectDirectory() / "project.enik");
}

void EditorLayer::ReloadProject() {
	ScriptSystem::ClearOnScriptModuleReloadEvents();
	if (m_SceneTreePanel.IsSelectedEntityValid()) {
		Enik::UUID selected_entity = m_SceneTreePanel.GetSelectedEntity().Get<Component::ID>().uuid;
		LoadProject(Project::GetActive()->GetProjectDirectory() / "project.enik");
		m_SceneTreePanel.SetSelectedEntityWithUUID(selected_entity);
	}
	else {
		LoadProject(Project::GetActive()->GetProjectDirectory() / "project.enik");
	}
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& event) {
	if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnKeyPressed(event);
	}

	if (event.IsRepeat()) {
		return false;
	}

	bool control = Input::IsKeyPressed(Key::LeftControl) or Input::IsKeyPressed(Key::RightControl);
	bool shift   = Input::IsKeyPressed(Key::LeftShift)   or Input::IsKeyPressed(Key::RightShift);

	switch (event.GetKeyCode()) {
		case Key::N:
			if (control) {
				DialogConfirm::OpenDialog("Create New Scene ?", BIND_FUNC(CreateNewScene));
			}
			break;

		case Key::O:
			if (control) {
				DialogFile::OpenDialog(DialogType::OPEN_FILE,
					[&](){
						LoadScene(DialogFile::GetSelectedPath());
					}, ".escn");
			}
			break;
		case Key::R:
			if (control) {
				ReloadProject();
			}
			break;

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
			if (control) {
				SaveScene();
				SceneSerializer serializer = SceneSerializer(m_ActiveScene);

				auto& id = m_SceneTreePanel.GetSelectedEntity().Get<Component::ID>().uuid;
				auto& new_id = serializer.DuplicateEntity(m_ActiveScenePath.string(), id);
				m_SceneTreePanel.SetSelectedEntityWithUUID(new_id);

			}
			break;

		case Key::Delete:
			if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0) {
				DialogConfirm::OpenDialog("Delete Entity ?",
					[&](){
						if (m_SceneTreePanel.IsSelectedEntityValid()) {
							m_ActiveScene->DestroyEntity(m_SceneTreePanel.GetSelectedEntity());
						}
					}
				);
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

		case Key::Q:
			if (control) {
				DialogConfirm::OpenDialog(
					"Exit Editor ?",
					[&](){
						OnSceneStop();
						Application::Get().Close();
					}
				);
			}
			break;

		case Key::F4:
			OnSceneStop();
			Application::Get().Close();
			break;



		default:
			break;
	}

	return false;
}

bool EditorLayer::OnKeyReleased(KeyReleasedEvent& event) {
    if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnKeyReleased(event);
	}
	return false;
}

bool EditorLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
	m_SceneTreePanel.OnMouseButtonReleased(event);

	if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnMouseButtonReleased(event);
	}
	return false;
}

bool EditorLayer::OnMouseScrolled(MouseScrolledEvent& event) {
    if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnMouseScrolled(event);
	}
	return false;
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
	if (m_ViewportHovered and m_SceneState == SceneState::Play) {
		m_ActiveScene->OnMouseButtonPressed(event);
	}
	return false;
}


// ? play pause stop buttons
void EditorLayer::ShowToolbarPlayPause() {
	static const float toolbar_min_size = 32.0f;
	static const float padding = 4.0f;
	static float toolbar_window_width = toolbar_min_size + padding;
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

	float size = ImGui::GetWindowHeight() - 4.0f;

	if (m_SceneState == SceneState::Play) {
		if (m_ActiveScene->IsPaused()) {
			auto texture_id_step = reinterpret_cast<void*>(static_cast<uintptr_t>(m_TextureStep->GetRendererID()));
			if (ImGui::ImageButton(texture_id_step, ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), 0)) {
				m_ActiveScene->Step();
			}
			ImGui::SameLine();
		}

		ImVec4 tint_color = ImVec4(1, 1, 1, 1);
		if (m_ActiveScene->IsPaused()) {
			tint_color = ImVec4(0.5f, 0.5f, 0.9f, 1.0f);
		}

		auto texture_id_pause = reinterpret_cast<void*>(static_cast<uintptr_t>(m_TexturePause->GetRendererID()));
		if (ImGui::ImageButton(texture_id_pause, ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), 0, ImVec4(0, 0, 0, 0), tint_color)) {
			OnScenePause(not m_ActiveScene->IsPaused());
		}
		ImGui::SameLine();
	}



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


	toolbar_window_width = size + padding;
	if (m_SceneState == SceneState::Play) {
		toolbar_window_width += size;
		if (m_ActiveScene->IsPaused()) {
			toolbar_window_width += size;
		}
	}

	ImGui::End();
	ImGui::PopStyleVar(4);
	ImGui::PopStyleColor(3);
}

void EditorLayer::OnScenePlay() {
	OnScenePause(false);
	SaveScene();

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

}

void EditorLayer::OnSceneStop() {
	OnScenePause(false);
	Enik::UUID current_selected_entity = m_SceneTreePanel.GetSelectedEntityUUID();

	m_SceneState = SceneState::Edit;
	m_ActiveScene = m_EditorScene;
	SetPanelsContext();

	m_SceneTreePanel.SetSelectedEntityWithUUID(current_selected_entity);
}

void EditorLayer::OnScenePause(bool is_paused) {
	if (m_SceneState == SceneState::Edit) {
		return;
	}
	m_ActiveScene->SetPaused(is_paused);
}

void EditorLayer::SetPanelsContext() {
	m_SceneTreePanel.SetContext(m_ActiveScene);
	m_InspectorPanel.SetContext(m_ActiveScene, &m_SceneTreePanel);
	m_FileSystemPanel.SetContext(m_ActiveScene);
	m_ToolbarPanel.SetContext(m_ActiveScene, &m_SceneTreePanel);
}

void EditorLayer::UpdateWindowTitle() {
	std::string window_title;

	if (m_ActiveScenePath.empty()) {
		window_title += "(*)";
	}
	else {
		window_title += m_ActiveScenePath.filename().string();
	}

	window_title += " - " + Project::GetActive()->GetConfig().project_name + " - enik engine";

	Application::SetWindowTitle(window_title);
}

void EditorLayer::OnOverlayRender() {
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
		auto view = m_ActiveScene->Reg().view<Component::Collider, Component::Transform>();
		for(auto& entity : view) {
			auto [transform, collider] = view.get<Component::Transform, Component::Collider>(entity);
			switch (collider.Shape) {
				case Component::ColliderShape::CIRCLE: {
					Renderer2D::DrawCircle(
						// transform.Position + collider.vector,
						glm::vec3(
							transform.GlobalPosition.x + collider.Vector.x,
							transform.GlobalPosition.y + collider.Vector.y,
							0.999f),
						transform.Scale.x * collider.Float,
						32,
						glm::vec4(0.3f, 0.8f, 0.3f, 1.0f));
					break;
				}
				case Component::ColliderShape::PLANE: {
					Component::Transform trans;
					trans.GlobalPosition = transform.GlobalPosition;
					trans.Rotation = transform.Rotation;
					trans.Scale = transform.Scale * collider.Float * 2.0f;
					trans.GlobalPosition.z = 0.999f;
					Renderer2D::DrawRect(trans, glm::vec4(0.3f, 0.8f, 0.3f, 1.0f));
					Renderer2D::DrawLine(
						trans.GlobalPosition,
						trans.GlobalPosition + glm::vec3(collider.Vector.x, collider.Vector.y, 0),
						glm::vec4(0.8f, 0.3f, 0.3f, 1.0f));
					break;
				}
				case Component::ColliderShape::BOX: {
					Component::Transform trans;
					trans.GlobalPosition = glm::vec3(
							transform.GlobalPosition.x + collider.Vector.x,
							transform.GlobalPosition.y + collider.Vector.y,
							0.999f);
					trans.Rotation = transform.Rotation;
					trans.Scale = transform.Scale * collider.Float * 2.0f;
					Renderer2D::DrawRect(trans, glm::vec4(0.3f, 0.8f, 0.3f, 1.0f));
					break;
				}

			}

		}

	}


	if (m_ShowSelectionOutline) {
		if (m_SceneTreePanel.IsSelectedEntityValid()) {
			auto transform = m_SceneTreePanel.GetSelectedEntity().Get<Component::Transform>();
			transform.Position.z = 0.999f;

			float increase_amount = 0.004f * m_EditorCameraController.GetZoomLevel();

			for (int i = 0; i < m_SelectionOutlineWidth; i++) {
				Renderer2D::DrawRect(
					transform,
					m_SelectionOutlineColor
				);
				transform.Scale.x += increase_amount;
				transform.Scale.y += increase_amount;
			}
		}
	}


	Renderer2D::EndScene();
}
