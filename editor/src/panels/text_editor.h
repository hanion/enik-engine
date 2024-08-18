#pragma once

#include "editor_panel.h"

#include <base.h>
#include <fstream>
#include <filesystem>
#include "scene/scene.h"

namespace Enik {

class TextEditorPanel : public EditorPanel {
public:
	TextEditorPanel() : EditorPanel("Text Editor") {}

	void RenderContent();

	bool OpenTextFile(const std::filesystem::path& path);
	bool SaveCurrentTextFile();

	const bool IsEditing() const { return not m_CurrentFile.empty(); }
	std::string CurrentFilePath() const { return m_CurrentFile.string(); }

	void AskToSaveFile();

private:
	std::filesystem::path m_CurrentFile;
	std::vector<char> m_Buffer = {0};

	bool m_WindowFocused = false;
};

}