#pragma once
#include "scene_editor_tab.h"

namespace Enik {

class PrefabEditorTab : public SceneEditorTab {
public:
	PrefabEditorTab(const std::filesystem::path& name);
	~PrefabEditorTab() {}

private:
	virtual void LoadScene(const std::filesystem::path& path) override final;
	virtual void SaveScene() override final;


	virtual void SetContext(EditorLayer* editor) override final {
		SceneEditorTab::SetContext(editor);
		m_SceneTreePanel.SetSelectedEntity(m_EditorRootEntity);
	}


	virtual void OnScenePlay() override final;
	virtual void OnSceneStop() override final;

	Ref<Scene> CreateNewScene();
private:
	Entity m_EditorRootEntity;
	Entity m_ActiveRootEntity;
	std::filesystem::path m_PrefabSourcePath;

};

}
