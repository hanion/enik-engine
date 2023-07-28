#pragma once

#include <base.h>
#include "window.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "events/application_event.h"
#include <log.h>
#include "layers/layer_stack.h"
#include "layers/imgui_layer/imgui_layer.h"
#include "renderer/opengl/shader.h"
#include "renderer/buffer.h"
#include "renderer/vertex_array.h"
#include "renderer/orthographic_camera.h"


namespace Enik {

class Application {

public:
	Application();
	virtual ~Application();
	virtual void Run();

	void OnEvent(Event& e);

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	inline static Application& Get() { return *s_Instance; }
	inline Window& GetWindow() { return *m_Window; }

private:
	bool OnWindowClose(WindowCloseEvent& e);

	Window* m_Window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	LayerStack m_LayerStack;

	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<VertexArray> m_VertexArray;
public:
	OrthographicCamera m_Camera;

private:
	static Application* s_Instance;
};

// To be defined in CLIENT
Application* CreateApplication();

}