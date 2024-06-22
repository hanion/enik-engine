#include "runtime.h"
#include "scene/scene_serializer.h"
#include <imgui/imgui_internal.h>


#define BIND_FUNC_EVENT(fn) std::bind(&RuntimeLayer::fn, this, std::placeholders::_1)


RuntimeLayer::RuntimeLayer()
	: Layer("RuntimeLayer") {
}

void RuntimeLayer::OnAttach() {
	EN_PROFILE_SCOPE;

	FrameBufferSpecification specification;
	specification.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::Depth };
	m_FrameBuffer = FrameBuffer::Create(specification);

	std::filesystem::path project = PROJECT_PATH;

	if (not std::filesystem::exists(project)) {
		EN_CORE_ERROR("Project not found! {}", project);
		return;
	}

	LoadProject(project);
}

void RuntimeLayer::OnDetach() {
	EN_PROFILE_SCOPE;

	m_ActiveScene->ClearNativeScripts();
	ScriptSystem::UnloadScriptModule();
}

void RuntimeLayer::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	if (m_ActiveScene == nullptr) {
		return;
	}

	m_Timestep = timestep;

	Renderer2D::ResetStats();

	m_FrameBuffer->Bind();

	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	RenderCommand::Clear();

	m_ActiveScene->OnUpdateRuntime(m_Timestep);

	m_FrameBuffer->Unbind();
}

void RuntimeLayer::OnFixedUpdate() {
	m_ActiveScene->OnFixedUpdate();
}

void RuntimeLayer::OnEvent(Event& event) {
	EventDispatcher dispatcher = EventDispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>         (BIND_FUNC_EVENT(OnKeyPressed));
	dispatcher.Dispatch<KeyReleasedEvent>        (BIND_FUNC_EVENT(OnKeyReleased));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_FUNC_EVENT(OnMouseButtonReleased));
	dispatcher.Dispatch<MouseButtonPressedEvent> (BIND_FUNC_EVENT(OnMouseButtonPressed));
	dispatcher.Dispatch<MouseScrolledEvent>      (BIND_FUNC_EVENT(OnMouseScrolled));
}

void RuntimeLayer::OnImGuiRender() {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	constexpr ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoSavedSettings
	;
	constexpr ImGuiWindowFlags viewport_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoSavedSettings
	;

	if (ImGui::Begin("DockSpace", nullptr, dockspace_flags)) {
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("DockSpaceID");
			ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_NoTabBar);
		}

		if (ImGui::Begin("Viewport", nullptr, viewport_flags)) {
			auto viewport_min_region = ImGui::GetWindowContentRegionMin();
			auto viewport_offset = ImGui::GetWindowPos();
			m_ViewportPosition.x = viewport_min_region.x + viewport_offset.x - viewport->Pos.x;
			m_ViewportPosition.y = viewport_min_region.y + viewport_offset.y - viewport->Pos.y;

			Application::Get().GetImGuiLayer()->BlockEvents(false);

			ImVec2 viewport_size = ImGui::GetContentRegionAvail();
			if (((viewport_size.x != m_ViewportSize.x) or (viewport_size.y != m_ViewportSize.y))
				and (viewport_size.x > 0 and viewport_size.y > 0)) {

				m_ViewportSize.x = viewport_size.x;
				m_ViewportSize.y = viewport_size.y;

				ResizeWindow();
			}

			uint32_t texture_id = m_FrameBuffer->GetColorAttachmentRendererID();
			ImGui::Image(
				reinterpret_cast<void*>(static_cast<uintptr_t>(texture_id)),
				ImVec2(m_ViewportSize.x, m_ViewportSize.y),
				ImVec2(0, 1), ImVec2(1, 0)
			);

			ImGui::End();
		}

		InitDockSpace();
		ImGui::End();
	}


#if RUNTIME_SHOW_DEBUG_INFO_PANEL
	m_DebugInfoPanel.ShowDebugInfoPanel(m_Timestep);
#endif

	ImGui::PopStyleVar(3);
}


void RuntimeLayer::InitDockSpace() {
	if (m_DockSpaceInitialized) {
		return;
	}

	ImGuiID dockspace_id = ImGui::GetID("DockSpaceID");
	ImGui::DockBuilderRemoveNode(dockspace_id);
	ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

	ImGuiID dock_id_main = dockspace_id;

	ImGui::DockBuilderDockWindow("Viewport", dock_id_main);
	ImGui::DockBuilderFinish(dockspace_id);

	m_DockSpaceInitialized = true;
}

void RuntimeLayer::ResizeWindow() {
	m_FrameBuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
	m_ActiveScene->OnViewportResize(m_ViewportPosition, m_ViewportSize.x, m_ViewportSize.y);
}

bool RuntimeLayer::OnKeyPressed(KeyPressedEvent& event) {
	m_ActiveScene->OnKeyPressed(event);
	return false;
}

bool RuntimeLayer::OnKeyReleased(KeyReleasedEvent& event) {
	m_ActiveScene->OnKeyReleased(event);

#if RUNTIME_SHOW_DEBUG_INFO_PANEL
	m_DebugInfoPanel.OnKeyReleased(event);
#endif

	return false;
}

bool RuntimeLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
	m_ActiveScene->OnMouseButtonPressed(event);
	return false;
}

bool RuntimeLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
		m_ActiveScene->OnMouseButtonReleased(event);
	return false;
}

bool RuntimeLayer::OnMouseScrolled(MouseScrolledEvent& event) {
	m_ActiveScene->OnMouseScrolled(event);
	return false;
}



void RuntimeLayer::LoadProject(const std::filesystem::path& path) {
	if (Project::Load(path)) {
		auto start_scene_path = Project::GetAbsolutePath(Project::GetActive()->GetConfig().start_scene);
		ScriptRegistry::ClearRegistry();
		ScriptSystem::LoadScriptModuleFirstTime();
		LoadScene(start_scene_path);
	}
}

void RuntimeLayer::LoadScene(const std::filesystem::path& path) {
	Ref<Scene> new_scene = CreateRef<Scene>();
	SceneSerializer serializer = SceneSerializer(new_scene);
	if (serializer.Deserialize(path.string())) {
		m_ActiveScene = new_scene;
		m_ActiveScene->OnViewportResize(m_ViewportPosition, m_ViewportSize.x, m_ViewportSize.y);
	}
}
