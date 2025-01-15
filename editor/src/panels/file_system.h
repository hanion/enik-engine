#pragma once
#include <base.h>

#include "editor_panel.h"
#include <filesystem>
#include "filewatch/FileWatch.hpp"


namespace Enik {

class EditorLayer;
class SceneEditorTab;

class FileSystemPanel : public EditorPanel {
public:
	FileSystemPanel() : EditorPanel("File System") {}
	~FileSystemPanel() = default;

	void SetContext(EditorLayer* editor, SceneEditorTab* tab);
	void SetCurrentDir(const std::filesystem::path& dir) { m_CurrentDirectory = dir;  m_HasSearched = false; }

private:
	virtual void RenderContent() override final;

	void SearchDirectory();
	void ShowDirectoriesTable();

	void ChangeDirectory(const std::filesystem::path& directory);

private:
	std::filesystem::path m_CurrentDirectory;
	std::string m_CurrentDirectoryText = "res://";

	EditorLayer* m_EditorLayer = nullptr;
	SceneEditorTab* m_EditorTab = nullptr;

	std::vector<std::filesystem::directory_entry> m_Entries;
	Scope<filewatch::FileWatch<std::string>> m_FileWatcher;

	bool m_HasSearched = false;

	const std::vector<std::string> m_Filters = {".escn", ".png", ".enik", ".prefab", ".wav", ".anim", ".ttf"};

};

}