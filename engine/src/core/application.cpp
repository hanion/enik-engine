#include "application.h"
#include "core/input.h"
#include <imgui/imgui.h>

#include "renderer/renderer.h"

namespace Enik {

// make application static
Application* Application::s_Instance = nullptr;


Application::Application(const std::string& name) {
	EN_PROFILE_SCOPE;

	EN_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	m_Window = CreateScope<Window>(WindowProperties(name,1280,600));
	m_Window->SetEventCallback(EN_BIND_EVENT_FN(Application::OnEvent));
	m_Window->SetVsync(false);

	Renderer::Init();

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
}

Application::~Application() {

}


void Application::PushLayer(Layer* layer) {
	EN_PROFILE_SCOPE;

	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay) {
	EN_PROFILE_SCOPE;

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
				EN_PROFILE_SECTION("layers OnUpdate");
				layer->OnUpdate(timestep);
			}
		}

		m_ImGuiLayer->Begin();
		for (Layer* layer : m_LayerStack) {
			EN_PROFILE_SECTION("imgui layers OnUpdate");
			layer->OnImGuiRender();
		}

		m_ImGuiLayer->End();

		m_Window->OnUpdate();
		EN_PROFILE_FRAME("Application::Run");
	}
}


void Application::OnEvent(Event& e) {
	EN_PROFILE_SCOPE;

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(EN_BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(EN_BIND_EVENT_FN(Application::OnWindowResize));
	dispatcher.Dispatch<KeyPressedEvent>(EN_BIND_EVENT_FN(Application::OnKeyPressed));

	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
		(*--it)->OnEvent(e);
		if (e.Handled) {
			break;
		}
	}
	
}

void Application::Close() {
	m_Running = false;
}

bool Application::OnWindowClose(WindowCloseEvent& e){
	Close();
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e){
	EN_PROFILE_SCOPE;

	if (e.GetWidth() == 0 ||  e.GetHeight() == 0) {
		m_Minimized = true;
		return false;
	}

	m_Minimized = false;
	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

	return false;
}

bool Application::OnKeyPressed(KeyPressedEvent& e) {
	bool control = Input::IsKeyPressed(Key::LeftControl) or Input::IsKeyPressed(Key::RightControl);
	
	if (control and e.GetKeyCode() == Key::P) {
		m_Minimized = !m_Minimized;
		EN_CORE_WARN("Paused Render (Ctrl+P)");
	}
	return false;
}
}