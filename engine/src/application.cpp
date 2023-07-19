#include "application.h"
#include "input.h"

namespace Enik {

// make application static
Application* Application::s_Instance = nullptr;


Application::Application() {
	EN_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

    m_Window = new Window(WindowProperties("eengine",1280,600));
	m_Window->SetEventCallback(EN_BIND_EVENT_FN(Application::OnEvent));

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
		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate();
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


}