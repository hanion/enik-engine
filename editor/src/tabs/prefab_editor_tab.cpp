#include "pch.h"
#include "prefab_editor_tab.h"
#include "asset/asset_manager_editor.h"
#include "scene/components.h"
#include "scene/scene_serializer.h"
#include "scene/scene.h"
#include "asset/importer/texture_importer.h"

namespace Enik {

PrefabEditorTab::PrefabEditorTab(const std::filesystem::path& name)
	: SceneEditorTab(name) {
	LoadScene(Project::GetAbsolutePath(name));
}

void PrefabEditorTab::LoadScene(const std::filesystem::path& path) {
	if (not std::filesystem::exists(path)) {
		return;
	}

	m_PrefabSourcePath = path;
	Ref<Scene> new_scene = CreateNewScene();
	m_EditorScene = new_scene;
	m_ActiveScene = m_EditorScene;
	m_ActiveScenePath = m_PrefabSourcePath;
	m_EditorRootEntity = m_ActiveRootEntity;
}

void PrefabEditorTab::SaveScene() {
	SceneSerializer serializer(m_EditorScene);
	serializer.CreatePrefab(m_PrefabSourcePath.string(), m_EditorRootEntity);
	m_EditorScene->SetName(m_EditorRootEntity.Get<Component::Tag>().Text);
	Project::GetAssetManagerEditor()->SerializeAssetRegistry();
}

Ref<Scene> PrefabEditorTab::CreateNewScene() {
	if (m_PrefabSourcePath.empty()) {
		return nullptr;
	}

	Ref<Scene> new_scene = CreateRef<Scene>();
	new_scene->OnViewportResize(m_ViewportPosition, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

	SceneSerializer serializer(new_scene);
	m_ActiveRootEntity = serializer.InstantiatePrefab(m_PrefabSourcePath.string(), 0, true);

	m_ActiveRootEntity.Remove<Component::Prefab>();
	new_scene->SetName(m_ActiveRootEntity.Get<Component::Tag>().Text);

	std::stack<Entity> all_entities;
	all_entities.push(m_ActiveRootEntity);

	while (not all_entities.empty()) {
		Entity current_entity = all_entities.top();
		all_entities.pop();

		if (current_entity.HasFamily()) {
			for (Entity& child : current_entity.GetChildren()) {
				if (!child.GetOrAdd<Component::Prefab>().RootPrefab) {
					child.Remove<Component::Prefab>();
					all_entities.push(child);
				}
			}
		}
	}
	return new_scene;
}


void PrefabEditorTab::OnScenePlay() {
	if (m_ActiveScenePath.empty() || m_PrefabSourcePath.empty()) {
		return;
	}
 	SaveScene();

	Enik::UUID current_selected_entity = m_SceneTreePanel.GetSelectedEntityUUID();

	m_ActiveScene = CreateNewScene();
	m_ActiveScene->OnViewportResize(m_ViewportPosition, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	SetPanelsContext();

	m_SceneTreePanel.SetSelectedEntityWithUUID(current_selected_entity);
	ImGui::SetWindowFocus(m_ViewportPanelName.c_str());

 	OnScenePause(false);
	m_SceneState = SceneState::Play;
}
void PrefabEditorTab::OnSceneStop() {
	OnScenePause(false);
	m_SceneState = SceneState::Edit;

	Tween::ResetData();

	Enik::UUID current_selected_entity = m_SceneTreePanel.GetSelectedEntityUUID();

	m_ActiveScene = m_EditorScene;
	SetPanelsContext();

	m_SceneTreePanel.SetSelectedEntityWithUUID(current_selected_entity);
}

}
