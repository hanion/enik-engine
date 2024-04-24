#pragma once
#include <Enik.h>

#define RUNTIME_SHOW_DEBUG_INFO_PANEL 1

#if RUNTIME_SHOW_DEBUG_INFO_PANEL
#include "debug_info.h"
#endif


using namespace Enik;

class RuntimeLayer : public Layer {
public:
	RuntimeLayer();
	virtual ~RuntimeLayer() = default;

	virtual void OnAttach() override final;
	virtual void OnDetach() override final;

	virtual void OnUpdate(Timestep timestep) override final;
	virtual void OnFixedUpdate() override final;
	virtual void OnEvent(Event& event) override final;
	virtual void OnImGuiRender() override final;


	bool OnKeyPressed (KeyPressedEvent &event);
	bool OnKeyReleased(KeyReleasedEvent &event);
	bool OnMouseButtonPressed (MouseButtonPressedEvent  &event);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent &event);
	bool OnMouseScrolled(MouseScrolledEvent &event);

private:
	void LoadProject(const std::filesystem::path &path);
	void LoadScene  (const std::filesystem::path &path);

	void InitDockSpace();

	void ResizeWindow();

private:
	Timestep m_Timestep;

	Ref<FrameBuffer> m_FrameBuffer;
	Ref<Scene> m_ActiveScene;

	glm::vec2 m_ViewportSize = glm::vec2(0);
	glm::vec2 m_ViewportPosition = glm::vec2(0);

	bool m_DockSpaceInitialized = false;

#if RUNTIME_SHOW_DEBUG_INFO_PANEL
	DebugInfoPanel m_DebugInfoPanel;
#endif


};