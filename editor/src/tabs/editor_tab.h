#pragma once

#include "base.h"
#include "core/timestep.h"
#include "events/event.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Enik {

class EditorLayer;

class EditorTab {
public:
	explicit EditorTab(const std::filesystem::path& name) :
		m_TabID(++s_TabIDCounter), m_Name(name.string()), m_WindowName(m_Name + "##tab" + std::to_string(m_TabID)) {}
	virtual ~EditorTab() = default;

	void OnImGuiRender();
	void DockTo(ImGuiID dockspace_id);

	virtual void OnUpdate(Timestep timestep) {}
	virtual void OnFixedUpdate() {}
	virtual void OnEvent(Event& event) {}

	const std::string& GetName() const { return m_Name; }
	const std::string& GetWindowName() const { return m_WindowName; }
	inline void SetWindowName(const std::string& name) { m_WindowName = name + "##tab" + std::to_string(m_TabID); }

	bool ShouldClose() const { return not m_IsOpen and not m_IsDirty; }
	bool IsFocused() const { return m_IsOpen and m_IsFocused; }
	bool IsDirty() const { return m_IsDirty; }

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
	std::string m_WindowName;

	bool m_IsOpen = true;
	bool m_IsDirty = false;
	bool m_IsFocused = true;

	ImGuiID m_DockspaceID;

	EditorLayer* m_EditorLayer;

	bool m_NoTabBar = false;

private:
	static int s_TabIDCounter;
	
	bool m_DockspaceInitialized = false;

};


}

