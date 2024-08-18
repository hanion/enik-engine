#pragma once


#include "base.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Enik {

class EditorPanel {
public:
	explicit EditorPanel(const std::string& name) : m_PanelID(s_PanelIDCounter++), m_Name(name), m_WindowName(m_Name + "##pan" + std::to_string(m_PanelID)) {}
	virtual ~EditorPanel() = default;

	void OnImGuiRender();

	void DockTo(ImGuiID dockspace_id);

protected:
	int m_PanelID;
	const std::string m_Name;
	const std::string m_WindowName;

	bool BeginPanel();
	virtual void RenderContent() = 0;


private:
	static int s_PanelIDCounter;

};



}
