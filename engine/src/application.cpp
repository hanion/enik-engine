#include "application.h"

namespace Enik {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application() {
    m_Window = new Window(WindowProperties());
	m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}

Application::~Application() {

}


void Application::PushLayer(Layer* layer) {
	m_LayerStack.PushLayer(layer);
}

void Application::PushOverlay(Layer* overlay) {
	m_LayerStack.PushOverlay(overlay);
}


void Application::Run() {
    while (m_Running) {
		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate();
		}

        m_Window->OnUpdate();
    }
}


void Application::OnEvent(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
		(*--it)->OnEvent(e);
		if (e.Handled) {
			break;
		}
	}
	
}

bool Application::OnWindowClose(WindowCloseEvent& e){
	EN_CORE_TRACE("closing it {0}",e);
	m_Running = false;
	return true;
}


}