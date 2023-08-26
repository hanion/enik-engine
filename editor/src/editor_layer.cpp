#include <pch.h>
#include "editor_layer.h"

#include "renderer/opengl/opengl_shader.h"
#include <glm/gtc/matrix_transform.hpp>

#include "scene/scene_serializer.h"



EditorLayer::EditorLayer()
	: Layer("EditorLayer"), m_EditorCameraController(1280.0f/600.0f) {
	
}

void EditorLayer::OnAttach() {
	EN_PROFILE_SCOPE;

	FrameBufferSpecification spec;
	spec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
	m_FrameBuffer = FrameBuffer::Create(spec);

	m_TexturePlay = Texture2D::Create(FULL_PATH_EDITOR("assets/icons/play_button.png"));
	m_TextureStop = Texture2D::Create(FULL_PATH_EDITOR("assets/icons/stop_button.png"));

	CreateNewScene();

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
			if (m_ViewportHovered and m_ViewportFocused) {
				HandlePickEntityWithMouse();
			}
			break;
		case SceneState::Play:
			m_ActiveScene->OnUpdateRuntime(m_Timestep);
			break;
	}


	

	m_FrameBuffer->Unbind();
}

void EditorLayer::OnEvent(Event& event) {
	m_EditorCameraController.OnEvent(event);

	EventDispatcher dispatcher = EventDispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(std::bind(&EditorLayer::OnKeyPressed, this, std::placeholders::_1));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(std::bind(&EditorLayer::OnMouseButtonReleased, this, std::placeholders::_1));
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
					// TODO: prompt dialog confirm
					CreateNewScene();
				}
				
				if (ImGui::MenuItem("Open File")) {
					m_ShowFileDialogAs = DialogType::OPEN_FILE;
					m_IsDialogOpen = true;
				}
				
				if (ImGui::MenuItem("Save")) {
					if (m_ActiveScenePath.empty()) {
						m_ShowFileDialogAs = DialogType::SAVE_FILE;
						m_IsDialogOpen = true;
					}
					else {
						SaveScene();
					}
				}
				if (ImGui::MenuItem("Save As")) {
					m_ShowFileDialogAs = DialogType::SAVE_FILE;
					m_IsDialogOpen = true;
				}
				
				if (ImGui::MenuItem("Exit")) {
					Application::Get().Close();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {
				ImGui::Checkbox("Show Performance",    &m_ShowPerformance  );
				ImGui::Checkbox("Show Renderer Stats", &m_ShowRendererStats);

				ImGui::EndMenu();
			}

			
			ImGui::EndMenuBar();
		}


		OnImGuiDockSpaceRender();

		ImGui::End();
	}

	if (DialogFile::Show(m_IsDialogOpen, m_ShowFileDialogAs) == DialogResult::ACCEPT) {
		if (m_ShowFileDialogAs == DialogType::OPEN_FILE) {
			LoadScene(DialogFile::GetSelectedPath());
		}
		else if (m_ShowFileDialogAs == DialogType::SAVE_FILE) {
			m_ActiveScenePath = DialogFile::GetSelectedPath();
			SaveScene();
		}
	}

}

void EditorLayer::OnImGuiDockSpaceRender() {
	EN_PROFILE_SCOPE;

	m_SceneTreePanel.OnImGuiRender();
	m_InspectorPanel.OnImGuiRender();
	m_FileSystemPanel.OnImGuiRender();

	/*Viewport*/ {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
		ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f), ImGuiCond_FirstUseEver);
		ImGui::Begin("Viewport", nullptr, window_flags);
		ImGui::PopStyleVar(1);

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
		
		
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = glm::vec2(viewportSize.x, viewportSize.y);
		if (((viewportSize.x != m_ViewportSize.x) or (viewportSize.y != m_ViewportSize.y)) and (viewportSize.x > 0 and viewportSize.y > 0)){
			m_ViewportSize.x = viewportSize.x;
			m_ViewportSize.y = viewportSize.y;

			m_FrameBuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCameraController.OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(textureID)), 
			ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0,1), ImVec2(1,0));
		

		/* Drag drop target */ {
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH",ImGuiDragDropFlags_AcceptBeforeDelivery)) {
					std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
					
					if (std::filesystem::exists(path) and path.extension() == ".escn") {

						// draw rect to show it can be draggable
						ImVec2 drawStart = ImVec2(m_ViewportBounds[0].x+2, m_ViewportBounds[0].y+2);
						ImVec2 drawEnd =   ImVec2(m_ViewportBounds[1].x-2, m_ViewportBounds[1].y-2);
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
		window_flags |=  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		window_flags |=  ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking;
		
		ImGui::SetNextWindowBgAlpha(0.65f);
		ImVec2 pos;
		pos.x = 20 + m_ViewportBounds[0].x;
		pos.y = 40 + m_ViewportBounds[0].y;

		ImGui::SetNextWindowPos(pos);
		if (ImGui::Begin("Performance", nullptr, window_flags))
		{
			ImGui::Text("Performance");
			ImGui::Text("deltaTime = %.2fms", m_Timestep.GetMilliseconds());
			ImGui::Text("FPS = %.0f", (1.0f/m_Timestep.GetSeconds()));
		}
		ImGui::End();
	}


	/* ShowRenderer2DStats */
	if (m_ShowRendererStats) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		window_flags |=  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		window_flags |=  ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking;
		
		ImGui::SetNextWindowBgAlpha(0.65f);
		ImVec2 pos;
		pos.x = 20  + m_ViewportBounds[0].x;
		pos.y = 130 + m_ViewportBounds[0].y;

		ImGui::SetNextWindowPos(pos);
		if (ImGui::Begin("Renderer2D Stats", nullptr, window_flags))
		{
			auto stats = Renderer2D::GetStats();

			ImGui::Text("Renderer2D Stats");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quad Count: %d", stats.QuadCount);
			ImGui::Text("Total Vertex Count: %d", stats.GetTotalVertexCount());
			ImGui::Text("Total Index  Count: %d", stats.GetTotalIndexCount());
		}
		ImGui::End();
	}


	ShowToolbar();
}


void EditorLayer::CreateNewScene() {
	m_ActiveScene = CreateRef<Scene>();
	SetPanelsContext();

	// create a new random path, 
	// maybe this is too much random ?
	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(20) << (uint64_t)UUID() << ".escn";
	m_ActiveScenePath = m_ActiveScenePath.parent_path() / std::filesystem::path(oss.str());
}

void EditorLayer::LoadScene(const std::filesystem::path& path) {
	if (m_SceneState != SceneState::Edit) {
		// TODO popup a confirmation dialog
		OnSceneStop();
	}

	Ref<Scene> newScene = CreateRef<Scene>();
	m_ActiveScenePath = path;
	SceneSerializer serializer = SceneSerializer(newScene);
	if (serializer.Deserialize(m_ActiveScenePath)) {
		m_EditorScene = newScene;
		m_ActiveScene = m_EditorScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		SetPanelsContext();
	}
}

void EditorLayer::SaveScene() {
	SceneSerializer serializer = SceneSerializer(m_ActiveScene);
	serializer.Serialize(m_ActiveScenePath);
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& event) {
	if (event.IsRepeat()) {
		return false;
	}

	bool control = Input::IsKeyPressed(Key::LeftControl) or Input::IsKeyPressed(Key::RightControl);
	bool shift   = Input::IsKeyPressed(Key::LeftShift  ) or Input::IsKeyPressed(Key::RightShift  );

	switch (event.GetKeyCode()) {
		case Key::N:
			if (control) { 
				CreateNewScene();
			}
			break;

		case Key::O:
			if (control) {
				m_ShowFileDialogAs = DialogType::OPEN_FILE;
				m_IsDialogOpen = true;
			}
			break;

		case Key::S:
			if (control and shift) {
				m_ShowFileDialogAs = DialogType::SAVE_FILE;
				m_IsDialogOpen = true;
			}
			else if (control) {
				SaveScene();
			}
			break;

		default:
			break;
	}

	return false;
}

bool EditorLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
	if (event.GetMouseButton() == Mouse::ButtonLeft) {
		m_PickEntityWithMouse = true;
	}
	return false;
}

void EditorLayer::HandlePickEntityWithMouse() {
	if (not m_PickEntityWithMouse) { 
		return;
	}

	EN_PROFILE_SCOPE;

	ImVec2 mousePos = ImGui::GetMousePos();
	mousePos.x -= m_ViewportBounds[0].x;
	mousePos.y -= m_ViewportBounds[0].y;
	
	glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	
	mousePos.y  = viewportSize.y - mousePos.y;
	
	int mouseX = (int)mousePos.x;
	int mouseY = (int)mousePos.y;

	if (mouseX >= 0 and mouseY >= 0 and mouseX < (int)viewportSize.x and mouseY < (int)viewportSize.y) {
		int pixelData = m_FrameBuffer->ReadPixel(1,mouseX, mouseY);
		if (pixelData == -1) {
			m_SceneTreePanel.SetSelectedEntity(Entity());
		}
		else {
			m_SceneTreePanel.SetSelectedEntity(Entity((entt::entity)pixelData, m_ActiveScene.get()));
		}
	}

	m_PickEntityWithMouse = false;
}



void EditorLayer::ShowToolbar() {
	static const float toolbarMinSize = 32.0f;
	static const float padding = 4.0f;
	static float toolbarWindowWidth = 0.0f;
	ImVec2 pos;
	pos.x = m_ViewportBounds[1].x - toolbarWindowWidth - padding;
	pos.y = m_ViewportBounds[0].y + padding;
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowBgAlpha(0.65f);

	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize,    ImVec2(toolbarMinSize,toolbarMinSize));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(2 ,2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,      ImVec2(0 ,0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0 ,0));
	ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize;
	flags |=  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;

	if (not ImGui::Begin("##Toolbar",nullptr, flags)) {
		ImGui::End();
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(3);
		return;
	}
	
	toolbarWindowWidth = ImGui::GetWindowSize().x;

	float size = ImGui::GetWindowHeight() - 4.0f;
	Ref<Texture2D> texture = (m_SceneState == SceneState::Edit) ? m_TexturePlay : m_TextureStop;
	auto textureID = reinterpret_cast<void*>(static_cast<uintptr_t>(texture->GetRendererID()));
	if (ImGui::ImageButton(textureID, ImVec2(size,size), ImVec2(0,1), ImVec2(1,0),0)) {
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
	m_SceneState = SceneState::Play;

	/* Copy Current Editor Scene */ {
		SaveScene();
		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer = SceneSerializer(newScene);
		if (serializer.Deserialize(m_ActiveScenePath)) {
			m_ActiveScene = newScene;
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			SetPanelsContext();
		}
	}
}

void EditorLayer::OnSceneStop() {
	m_SceneState = SceneState::Edit;
	m_ActiveScene = m_EditorScene;
	SetPanelsContext();
}

void EditorLayer::SetPanelsContext() {
	m_SceneTreePanel.SetContext(m_ActiveScene);
	m_InspectorPanel.SetContext(m_ActiveScene, &m_SceneTreePanel);
	m_FileSystemPanel.SetContext(m_ActiveScene);
}
