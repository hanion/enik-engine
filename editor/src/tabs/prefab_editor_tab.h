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

private:
	Entity m_RootEntity;
	std::filesystem::path m_PrefabSourcePath;

};

}
