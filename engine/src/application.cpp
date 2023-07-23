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

	// Vertex Array
	// Vertex Buffer
	// Index Buffer

	glGenVertexArrays(1, &m_VertexArray);
	glBindVertexArray(m_VertexArray);

	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

	float vertices[3 * 3] = {
		0.0f,  0.8f,  0.0f,
		-0.5f, -0.2f, 0.0f,
		0.5f,  -0.2f, 0.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

	unsigned int indices[3] = { 0, 1, 2 };

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	std::string vertexSource = R"(
		#version 330 core

		layout(location = 0) in vec3 a_Position;
		
		out vec3 v_Position;

		void main() {
			v_Position = a_Position;
			gl_Position = vec4(a_Position, 1.0);
		}

	)";

	std::string fragmentSource = R"(
		#version 330 core

		layout(location = 0) out vec4 color;
		in vec3 v_Position;

		void main() {
			//color = vec4(0.5, 0.3, 0.3, 1.0);
			color = vec4(v_Position * 0.5 + 0.5, 1.0);

		}

	)";

	m_Shader.reset(new Shader(vertexSource, fragmentSource));
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

		glClearColor(0.1f, 0.1f, 0.1f, 1);
		glClear(GL_COLOR_BUFFER_BIT);


		m_Shader->Bind();
		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

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