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

	Ref<Scene> new_scene = CreateRef<Scene>();
	m_EditorScene = new_scene;
	m_ActiveScene = m_EditorScene;
	m_ActiveScenePath = "";
	m_ActiveScene->OnViewportResize(m_ViewportPosition, (uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	SetPanelsContext();

	SceneSerializer serializer(new_scene);
	m_RootEntity = serializer.InstantiatePrefab(path.string(), 0, true);

	m_RootEntity.Remove<Component::Prefab>();
	new_scene->SetName(m_RootEntity.Get<Component::Tag>().Text);
	m_SceneTreePanel.SetSelectedEntity(m_RootEntity);


	std::stack<Entity> all_entities;
	all_entities.push(m_RootEntity);

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
}

void PrefabEditorTab::SaveScene() {
	SceneSerializer serializer(m_EditorScene);
	serializer.CreatePrefab(m_PrefabSourcePath.string(), m_RootEntity);
	m_EditorScene->SetName(m_RootEntity.Get<Component::Tag>().Text);
	Project::GetAssetManagerEditor()->SerializeAssetRegistry();
}


}
