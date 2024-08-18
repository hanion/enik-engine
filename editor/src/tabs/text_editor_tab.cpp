#include "text_editor_tab.h"

namespace Enik {

TextEditorTab::TextEditorTab(const std::string& name) : EditorTab(name) {
	OpenTextFile(name);
}

void TextEditorTab::RenderContent() {
	m_Panel.OnImGuiRender();
}

void TextEditorTab::InitializeDockspace() {
	m_Panel.DockTo(m_DockspaceID);
}

}
