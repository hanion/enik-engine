#include "application.h"
#include "core/input.h"
#include <imgui.h>

#include "renderer/renderer.h"

namespace Enik {

// make application static
Application* Application::s_Instance = nullptr;


Application::Application() {
	EN_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	m_Window = new Window(WindowProperties("eengine",1280,600));
	m_Window->SetEventCallback(EN_BIND_EVENT_FN(Application::OnEvent));
	m_Window->SetVsync(false);

	Renderer::Init();

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
}

Application::~Application() {

}


void Application::PushLayer(Layer* layer) {
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay) {
	m_LayerStack.PushOverlay(overlay);
	overlay->OnAttach();
}


void Application::Run() {
	while (m_Running) {
		float time = (float)glfwGetTime();
		Timestep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		if (!m_Minimized) {
			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate(timestep);
			}
		}

		m_ImGuiLayer->Begin();
		for (Layer* layer : m_LayerStack) {
			layer->OnImGuiRender();
		}

		m_ImGuiLayer->End();

		m_Window->OnUpdate();
	}
}


void Application::OnEvent(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(EN_BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(EN_BIND_EVENT_FN(Application::OnWindowResize));

	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
		(*--it)->OnEvent(e);
		if (e.Handled) {
			break;
		}
	}
	
}

bool Application::OnWindowClose(WindowCloseEvent& e){
	m_Running = false;
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e){
	if (e.GetWidth() == 0 ||  e.GetHeight() == 0) {
		m_Minimized = true;
		return false;
	}

	m_Minimized = false;
	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

	return false;
}

}