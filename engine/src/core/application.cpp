#include "application.h"

#include <imgui/imgui.h>

#include "core/input.h"
#include "renderer/renderer.h"
#include "physics/physics.h"
#include "audio/audio.h"

namespace Enik {

// make application static
Application* Application::s_Instance = nullptr;
Application& Application::Get() { return *Application::s_Instance; }

Application::Application(const std::string& name) {
	EN_PROFILE_SCOPE;

	EN_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	m_Window = CreateScope<Window>(WindowProperties(name, 1600, 800));
	m_Window->SetEventCallback(EN_BIND_EVENT_FN(Application::OnEvent));
	m_Window->SetVsync(true);

	Renderer::Init();

	Audio::Init();

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
	const double dt = PHYSICS_UPDATE_RATE;
	double accumulator = 0.0;

	while (m_Running) {
		float time = (float)glfwGetTime();
		Timestep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		ExecuteMainThreadQueue();

		if (!m_Minimized) {
			accumulator += timestep.GetSeconds();
			while ( accumulator >= dt ) {
				for (Layer* layer : m_LayerStack) {
					EN_PROFILE_SECTION("layers OnFixedUpdate");
					layer->OnFixedUpdate();
				}
				accumulator -= dt;
			}

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

	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
		(*--it)->OnEvent(e);
		if (e.Handled) {
			break;
		}
	}
}

void Application::Close() {
	m_Running = false;
}

void Application::SubmitToMainThread(const std::function<void()>& function) {
	std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
	m_MainThreadQueue.emplace_back(function);
}

bool Application::OnWindowClose(WindowCloseEvent& e) {
	Close();
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e) {
	EN_PROFILE_SCOPE;

	if (e.GetWidth() == 0 || e.GetHeight() == 0) {
		m_Minimized = true;
		return false;
	}

	m_Minimized = false;
	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

	return false;
}

bool Application::OnKeyPressed(KeyPressedEvent& e) {
	return false;
}

void Application::ExecuteMainThreadQueue() {
	std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
	for(auto& function : m_MainThreadQueue) {
		function();
	}
	m_MainThreadQueue.clear();
}

}