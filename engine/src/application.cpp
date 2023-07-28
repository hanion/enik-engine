#include "application.h"
#include "input.h"

#include "renderer/renderer.h"

namespace Enik {

// make application static
Application* Application::s_Instance = nullptr;


Application::Application() 
	: m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {
	EN_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	m_Window = new Window(WindowProperties("eengine",1280,600));
	m_Window->SetEventCallback(EN_BIND_EVENT_FN(Application::OnEvent));

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);

	m_VertexArray.reset(VertexArray::Create());

	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};


	std::shared_ptr<VertexBuffer> vertexBuffer;
	vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
	
	
	BufferLayout layout = {
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float4, "a_Color"}
	};
	
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[3] = { 0, 1, 2 };
	
	std::shared_ptr<IndexBuffer> indexBuffer;
	indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));
	m_VertexArray->SetIndexBuffer(indexBuffer);







	std::string vertexSource = R"(
		#version 330 core

		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec4 a_Color;

		uniform mat4 u_ViewProjection;
		
		out vec3 v_Position;
		out vec4 v_Color;

		void main() {
			v_Position = a_Position;
			v_Color = a_Color;
			gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
		}

	)";

	std::string fragmentSource = R"(
		#version 330 core

		layout(location = 0) out vec4 color;
		in vec3 v_Position;
		in vec4 v_Color;


		void main() {
			//color = vec4(0.5, 0.3, 0.3, 1.0);
			color = vec4(v_Position * 0.5 + 0.5, 1.0);
			color = v_Color;
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
		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		RenderCommand::Clear();

		Renderer::BeginScene(m_Camera);		
		Renderer::Submit(m_Shader, m_VertexArray);
		/*
		Renderer::Submit()
			RenderCommand::DrawIndexed
				RenderAPI::
					OpenGLRendererAPI::DrawIndexed
						glDrawElements();
		*/

		Renderer::EndScene();

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