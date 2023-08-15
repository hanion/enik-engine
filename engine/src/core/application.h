#pragma once

#include <base.h>
#include "window.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "events/application_event.h"
#include "core/log.h"
#include "layers/layer_stack.h"
#include "layers/imgui_layer/imgui_layer.h"
#include "core/timestep.h"


namespace Enik {

class Application {

public:
	Application(const std::string& name = "eengine");
	virtual ~Application();
	virtual void Run();

	void OnEvent(Event& e);

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	inline static Application& Get() { return *s_Instance; }
	inline Window& GetWindow() { return *m_Window; }
	
	inline ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

	void Close();

private:
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
	bool OnKeyPressed(KeyPressedEvent& e);
private:
	Window* m_Window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	bool m_Minimized = false;
	LayerStack m_LayerStack;
	float m_LastFrameTime = 0.0f;

private:
	static Application* s_Instance;
};

// To be defined in CLIENT
Application* CreateApplication();

}