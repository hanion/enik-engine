#include "sandbox2D.h"

#include "renderer/opengl/opengl_shader.h"

#define FULL_PATH(x) (((std::string)"/home/han/dev/enik-engine/sandbox/") + ((std::string)x))


Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1.6f/0.75f, true) {
	
}

void Sandbox2D::OnAttach() {
	m_VertexArray = VertexArray::Create();

	float vertices[4 * 9] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};


	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	
	
	BufferLayout layout = {
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float2, "a_TexCoord"}
	};
	
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
	
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	/*Create shader sources*/{
		
		// m_Shader = Shader::Create(FULL_PATH("assets/shaders/colorful.glsl"));
		m_Shader = Shader::Create(FULL_PATH("assets/shaders/texture.glsl"));
		std::dynamic_pointer_cast<OpenGLShader>(m_Shader)->UploadUniformInt("u_Texture", 0);
		
		m_Texture2D = Texture2D::Create(FULL_PATH("assets/textures/checkerboard.png"));
		// m_Texture2D = Texture2D::Create(FULL_PATH("assets/textures/tablordia_banner.png"));
	}
}

void Sandbox2D::OnDetach() {

}

void Sandbox2D::OnUpdate(Timestep timestep) {
	m_Timestep = timestep;

	m_CameraController.OnUpdate(m_Timestep);

	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	RenderCommand::Clear();

	Renderer::BeginScene(m_CameraController.GetCamera());
	
	m_Texture2D->Bind();
	Renderer::Submit(m_Shader, m_VertexArray, glm::mat4(1.0f));


	Renderer::EndScene(); 
}

void Sandbox2D::OnEvent(Event& event) {
	m_CameraController.OnEvent(event);
}

void Sandbox2D::OnImGuiRender() {
	/*ShowPerformance*/ {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		window_flags |=  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		
		ImGui::SetNextWindowBgAlpha(0.35f);
		if (ImGui::Begin("Performance", nullptr, window_flags))
		{
			ImGui::Text("deltaTime = %.2fms", m_Timestep.GetMilliseconds());
			ImGui::Text("FPS = %.0f", (1.0f/m_Timestep.GetSeconds()));
		}
		ImGui::End();
	}
}
