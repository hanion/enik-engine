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

	unsigned int m_VertexArray;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;

private:
	static Application* s_Instance;
};

// To be defined in CLIENT
Application* CreateApplication();

}