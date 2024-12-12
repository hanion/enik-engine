#pragma once 

#include "editor_tab.h"
#include "panels/text_editor.h"


namespace Enik {

class TextEditorTab : public EditorTab {
public:
	TextEditorTab(const std::filesystem::path& name);

	bool OpenTextFile(const std::filesystem::path& path) {
		return m_Panel.OpenTextFile(path);
	}


private:
	virtual void RenderContent() override final;
	virtual void InitializeDockspace() override final;


private:
	TextEditorPanel m_Panel;


};



}
