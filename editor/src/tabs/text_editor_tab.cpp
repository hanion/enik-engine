#include "text_editor_tab.h"

namespace Enik {

TextEditorTab::TextEditorTab(const std::filesystem::path& name) : EditorTab(name) {
	m_NoTabBar = true;
	SetWindowName(name.filename().string());

	OpenTextFile(name);
}

void TextEditorTab::RenderContent() {
	m_Panel.OnImGuiRender();
}

void TextEditorTab::InitializeDockspace() {
	m_Panel.DockTo(m_DockspaceID);
}

}
