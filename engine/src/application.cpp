#include "application.h"

namespace Enik {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application() {
    m_Window = new Window(WindowProperties());
	m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}

Application::~Application() {

}

void Application::Run() {
    while (m_Running) {
        m_Window->OnUpdate();
    }
}


 bool Application::OnWindowClose(WindowCloseEvent& e){
	EN_CORE_TRACE("closing it {0}",e);
	m_Running = false;
	return true;
 }


}