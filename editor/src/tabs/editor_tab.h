#pragma once

#include "base.h"
#include "core/timestep.h"
#include "events/event.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

class EditorLayer;

namespace Enik {

class EditorTab {
public:
	explicit EditorTab(const std::string& name) : m_TabID(s_TabIDCounter++), m_Name(name), m_WindowName(m_Name + "##tab" + std::to_string(m_TabID)) {}
	virtual ~EditorTab() = default;

	void OnImGuiRender();
	void DockTo(ImGuiID dockspace_id);

	virtual void OnUpdate(Timestep timestep) {}
	virtual void OnFixedUpdate() {}
	virtual void OnEvent(Event& event) {}

	const std::string& GetName() const { return m_Name; }
	const std::string& GetWindowName() const { return m_WindowName; }

	bool ShouldClose() const { return not m_IsOpen and not m_IsDirty; }

	virtual void SetContext(EditorLayer* editor) {
		m_EditorLayer = editor;
	}

protected:
	bool BeginDockspace();
	virtual void RenderContent() {};
	virtual void InitializeDockspace() {};

protected:
	int m_TabID;
	const std::string m_Name;
	const std::string m_WindowName;

	bool m_IsOpen = true;
	bool m_IsDirty = false;

	ImGuiID m_DockspaceID;

	EditorLayer* m_EditorLayer;

private:
	static int s_TabIDCounter;
	
	bool m_DockspaceInitialized = false;

};


}

