#include "editor_layer.h"

#include <algorithm>
#include <filesystem>
#include <pch.h>

#include <glm/gtc/matrix_transform.hpp>

#include "base.h"
#include "core/log.h"
#include "dialogs/dialog_confirm.h"
#include "dialogs/dialog_file.h"
#include "scene/scene_serializer.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "tabs/editor_tab.h"
#include "tabs/prefab_editor_tab.h"
#include "tabs/scene_editor_tab.h"
#include "tabs/text_editor_tab.h"
#include "utils/editor_assets.h"
#include "utils/editor_colors.h"



#define BIND_FUNC(fn) std::bind(&EditorLayer::fn, this)
#define BIND_FUNC_EVENT(fn) std::bind(&EditorLayer::fn, this, std::placeholders::_1)

EditorLayer::EditorLayer()
	: Layer("EditorLayer") {
}

void EditorLayer::OnAttach() {
	EN_PROFILE_SCOPE;

	EditorAssets::LoadEditorAssets();

	std::filesystem::path project = PROJECT_PATH;
	if (std::filesystem::exists(project)) {
		LoadProject(project);
	}
}

void EditorLayer::OnDetach() {
	EN_PROFILE_SCOPE;

	m_EditorTabs.clear();
	ScriptSystem::UnloadScriptModule();
}

void EditorLayer::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	for (const auto& tab : m_EditorTabs) {
		if (tab->IsFocused()) {
			if (m_FocusedTab != tab) {
				if (auto set = std::dynamic_pointer_cast<SceneEditorTab>(m_FocusedTab)) {
					set->OnSceneStop();
				}
			}
			m_FocusedTab = tab;
		}
	}

	if (m_FocusedTab) {
		m_FocusedTab->OnUpdate(timestep);
	}
}

void EditorLayer::OnFixedUpdate() {
	if (m_FocusedTab) {
		m_FocusedTab->OnFixedUpdate();
	}
}



void EditorLayer::OnEvent(Event& event) {
	EventDispatcher dispatcher = EventDispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(BIND_FUNC_EVENT(OnKeyPressed));

	if (m_FocusedTab) {
		m_FocusedTab->OnEvent(event);
	}
}

void EditorLayer::OnImGuiRender() {
	if (BeginMainDockspace()) {
		RenderContent();
		InitializeMainDockspace();
	}
	ImGui::End();


	// closing tabs
	m_EditorTabs.erase(
		std::remove_if(m_EditorTabs.begin(), m_EditorTabs.end(),
			[](const auto& tab) {
				if (tab->ShouldClose()) {
					return true;
				}
				return false;
			}
		),
		m_EditorTabs.end()
	);

	DialogFile::Show();
	DialogConfirm::Show();
}


bool EditorLayer::BeginMainDockspace() {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	constexpr ImGuiWindowFlags main_window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::SetNextWindowClass(&m_MainDockspaceClass);
	ImGui::Begin("MainDockSpace", nullptr, main_window_flags);

	ImGui::PopStyleVar(3);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::EditorTabPadding);
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		m_MainDockspaceID = ImGui::GetID("MainDockSpaceID");
		ImGui::SetNextWindowClass(&m_MainDockspaceClass);
		ImGui::DockSpace(m_MainDockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoDockingSplitMe);
	}
	ImGui::PopStyleVar();


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
						}
						, ".enik"
					);
				}
				if (ImGui::MenuItem("Save Project")) {
					SaveProject();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Open Scene")) {
					DialogFile::OpenDialog(DialogType::OPEN_FILE,
						[&](){
							RequestOpenAsset(Project::GetRelativePath(DialogFile::GetSelectedPath()));
						}
						, ".escn"
					);
				}
				if (ImGui::MenuItem("New Scene")) {
					CreateNewScene();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	return true;
}


void EditorLayer::RenderContent() {
	// open requested assets
	ProcessOpenAssetRequests();

	for (const auto& tab : m_EditorTabs) {
		ImGui::SetNextWindowClass(&m_MainDockspaceClass);
		tab->OnImGuiRender();
	}
}

void EditorLayer::InitializeMainDockspace() {
	if (m_MainDockspaceInitialized) {
		return;
	}

	m_MainDockspaceID = ImGui::GetID("MainDockSpaceID");
	ImGui::DockBuilderRemoveNode(m_MainDockspaceID);
	ImGui::DockBuilderAddNode(m_MainDockspaceID, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(m_MainDockspaceID, ImGui::GetMainViewport()->Size);
	ImGui::DockBuilderFinish(m_MainDockspaceID);

	m_MainDockspaceClass.ClassId = ImGui::GetID("MainDockspaceClass");
	m_MainDockspaceClass.DockingAllowUnclassed = false;

	m_MainDockspaceInitialized = true;
}


void EditorLayer::RequestOpenAsset(const std::filesystem::path& path) {
	// do not open new if its already open
	for (const auto& tab : m_EditorTabs) {
		if (tab->GetName() == path) {
			ImGui::FocusWindow(ImGui::FindWindowByName(tab->GetWindowName().c_str()));
			return;
		}
	}

	m_OpenAssetRequests.push_back(path);
}

void EditorLayer::ProcessOpenAssetRequests() {
	if (not m_MainDockspaceInitialized) {
		return;
	}
	if (m_OpenAssetRequests.empty()) {
		return;
	}

	for (auto& path : m_OpenAssetRequests) {
		OpenAsset(path);
	}
	m_OpenAssetRequests.clear();
}

void EditorLayer::OpenAsset(const std::filesystem::path& path) {
	std::string ext = path.extension().string();
	std::string filename = path.filename().string();

	Ref<EditorTab> tab = nullptr;
	if (ext == ".escn") {
		tab = std::static_pointer_cast<EditorTab>(CreateRef<SceneEditorTab>(path));
	} else if (ext == ".prefab") {
		tab = std::static_pointer_cast<EditorTab>(CreateRef<PrefabEditorTab>(path));
	} else if (ext == ".enik" or
		ext == ".txt" or
		ext == ".png" or
		ext == ".anim" or
		ext == ".wav") {
		tab = std::static_pointer_cast<EditorTab>(CreateRef<TextEditorTab>(path));
	}

	if (tab) {
		tab->DockTo(m_MainDockspaceID);
		tab->SetContext(this);
		m_EditorTabs.push_back(tab);
		m_FocusedTab = tab;
		ImGui::FocusWindow(ImGui::FindWindowByName(tab->GetWindowName().c_str()));
	} else {
		EN_CORE_ERROR("Could not open tab! {}", path.string());
	}
}


void EditorLayer::CreateNewScene() {
	DialogFile::OpenDialog(DialogType::SAVE_FILE,
		[&](){
			std::filesystem::path new_path = DialogFile::GetSelectedPath();
			std::filesystem::path relative = Project::GetRelativePath(new_path);

			if (std::filesystem::exists(new_path)) {
				DialogConfirm::OpenDialog(
					"Override ?",
					relative.string() + " already exists.\nDo you want to override?",
					[&]() {
						Ref<Scene> new_scene = CreateRef<Scene>();
						SceneSerializer serializer = SceneSerializer(new_scene);
						serializer.Serialize(DialogFile::GetSelectedPath().string());
						RequestOpenAsset(Project::GetRelativePath(DialogFile::GetSelectedPath()));
					}
				);
			}
			else {
				Ref<Scene> new_scene = CreateRef<Scene>();
				SceneSerializer serializer = SceneSerializer(new_scene);
				serializer.Serialize(new_path.string());
				RequestOpenAsset(relative);
			}

		}
	, ".escn"
	);
}


void EditorLayer::CreateNewProject() {
	Project::New();
	DialogFile::OpenDialog(DialogType::SAVE_FILE,
		[&](){
			// TODO this
			EN_CORE_WARN("TODO Create a new project here. (EditorLayer::CreateNewProject)");
		}, ".enik");
}

void EditorLayer::LoadProject(const std::filesystem::path& path) {
	if (Project::Load(path)) {
		ScriptRegistry::ClearRegistry();
		ScriptSystem::LoadScriptModuleFirstTime();
		ScriptSystem::ClearOnScriptModuleReloadEvents();

		auto start_scene_path = Project::GetActive()->GetConfig().start_scene;
		RequestOpenAsset(start_scene_path);

		DialogFile::SetCurrentDir(Project::GetProjectDirectory());
		UpdateWindowTitle();
	}
}

void EditorLayer::SaveProject() {
	Project::Save(Project::GetActive()->GetProjectDirectory() / "project.enik");
}

void EditorLayer::ReloadProject() {
	if (Project::GetActive()) {
		ScriptSystem::ClearOnScriptModuleReloadEvents();
		LoadProject(Project::GetActive()->GetProjectDirectory() / "project.enik");
	}
}

void EditorLayer::ReloadTab() {
	for (auto it = m_EditorTabs.begin(); it != m_EditorTabs.end(); ++it) {
		Ref<EditorTab> tab = *it;
		if (tab->IsFocused() and !tab->IsDirty()) {
			auto name = tab->GetName();
			m_EditorTabs.erase(it);
			RequestOpenAsset(name);
			return;
		}
	}
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& event) {
	if (event.IsRepeat()) {
		return false;
	}

	bool control = Input::IsKeyPressed(Key::LeftControl) or Input::IsKeyPressed(Key::RightControl);

	switch (event.GetKeyCode()) {
		case Key::N:
			if (control) {
				CreateNewScene();
			}
			break;

		case Key::O:
			if (control) {
				DialogFile::OpenDialog(DialogType::OPEN_FILE,
					[&](){
						RequestOpenAsset(Project::GetRelativePath(DialogFile::GetSelectedPath()));
					}
					, ".escn"
				);
			}
			break;
		case Key::R:
			if (control) {
				ReloadTab();
			}
			break;

		case Key::Q:
			if (control) {
				ExitEditor();
			}
			break;

		case Key::F4:
			ExitEditor();
			break;

		case Key::T:
			if (control) {
				if (Project::GetActive()){
					RequestOpenAsset(Project::GetActive()->GetConfig().start_scene);
				}
			}
			break;

		default:
			break;
	}

	return false;
}




void EditorLayer::UpdateWindowTitle() {
	std::string window_title = Project::GetActive()->GetConfig().project_name + " - enik engine";
	Application::SetWindowTitle(window_title);
}

void EditorLayer::ExitEditor() {
	DialogConfirm::OpenDialog(
		"Exit Editor ?",
		"Everything not saved will be lost.",
		[&](){
			Application::Get().Close();
		}
	);
}
