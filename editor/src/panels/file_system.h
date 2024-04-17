#pragma once
#include <base.h>

#include <filesystem>
#include "filewatch/FileWatch.hpp"

#include "scene/scene.h"
#include "text_editor.h"

namespace Enik {

class FileSystemPanel {
public:
	FileSystemPanel() = default;
	~FileSystemPanel() = default;

	void SetContext(TextEditorPanel* text_editor_panel);
	void SetCurrentDir(const std::filesystem::path& dir) { m_CurrentDirectory = dir;  m_HasSearched = false; }

	void OnImGuiRender();

private:
	void SearchDirectory();
	void ShowDirectoriesTable();

	void ChangeDirectory(const std::filesystem::path& directory);

private:
	TextEditorPanel* m_TextEditorPanel;

	std::filesystem::path m_CurrentDirectory;
	std::string m_CurrentDirectoryText;

	std::vector<std::filesystem::directory_entry> m_Entries;
	Scope<filewatch::FileWatch<std::string>> m_FileWatcher;

	bool m_HasSearched = false;

	const std::vector<std::string> m_Filters = {".escn", ".png", ".enik", ".prefab"};
};

}