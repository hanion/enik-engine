#pragma once
#include <base.h>

#include <filesystem>

#include "scene/scene.h"

namespace Enik {

class FileSystemPanel {
public:
	FileSystemPanel() = default;
	FileSystemPanel(const Ref<Scene> context);
	~FileSystemPanel() = default;

	void SetContext(const Ref<Scene>& context);

	void OnImGuiRender();

private:
	void SearchDirectory();
	void ShowDirectoriesTable();

private:
	Ref<Scene> m_Context;

	// does not go further up than this path, do not change
	std::filesystem::path m_AssetsPath = std::filesystem::canonical("../../");
	std::filesystem::path m_CurrentPath = m_AssetsPath / "editor/assets";

	std::vector<std::filesystem::directory_entry> m_Entries;

	bool m_HasSearched = false;
};

}