#include <Enik.h>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/opengl/opengl_shader.h"

#define FULL_PATH(x) (((std::string)"/home/han/dev/enik-engine/sandbox/") + ((std::string)x))

using namespace Enik;

class ExampleLayer : public Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1.6f/0.75f, true) {
		m_VertexArray = VertexArray::Create();

		float vertices[4 * 9] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
		};


		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		
		
		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"}
		};
		
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
		
		Ref<IndexBuffer> indexBuffer;
		indexBuffer = IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		/*Create shader sources*/{			
			m_ShaderLibrary.Load("colorful", FULL_PATH("assets/shaders/colorful.glsl"));

			auto textureShader = m_ShaderLibrary.Load("texture", FULL_PATH("assets/shaders/texture2.glsl"));
			
			std::dynamic_pointer_cast<OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
			
			m_Texture2D = Texture2D::Create(FULL_PATH("assets/textures/checkerboard.png"));
			m_TransparentTexture2D = Texture2D::Create(FULL_PATH("assets/textures/tablordia_banner.png"));
		}
	}


	void OnUpdate(Timestep timestep) override {
		m_Timestep = timestep;

		m_CameraController.OnUpdate(m_Timestep);

		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		RenderCommand::Clear();

		Renderer::BeginScene(m_CameraController.GetCamera());

		/*Create mini squares*/ {
			auto colorfulShader = m_ShaderLibrary.Get("colorful");
			static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
			for (size_t i = 0; i < 5; i++) {
				glm::vec3 pos = glm::vec3(i * 0.2f - (2.0f*0.2f), -0.6f, 0.0f);
				glm::mat4 miniTransform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Renderer::Submit(colorfulShader, m_VertexArray, miniTransform);
			}
		}


		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_TexturePosition);
		
		m_Texture2D->Bind();
		auto textureShader = m_ShaderLibrary.Get("texture");
		Renderer::Submit(textureShader, m_VertexArray, glm::mat4(1.0f));
		
		m_TransparentTexture2D->Bind();
		Renderer::Submit(textureShader, m_VertexArray, glm::scale(transform, m_TextureScale));

		Renderer::EndScene(); 
	}

	void OnEvent(Event& event) override {
		m_CameraController.OnEvent(event);
	}

	virtual void OnImGuiRender() override {
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

		/*ShowTextureControls*/ {
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			if (ImGui::Begin("Texture", NULL, window_flags)) {

				ImGui::Text("Translation");
				ImGui::DragFloat("X",&m_TexturePosition.x, 0.02f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat("Y",&m_TexturePosition.y, 0.02f, -10.0f, 10.0f, "%.2f");
			
				ImGui::Text("Scale");
				ImGui::DragFloat("x",&m_TextureScale.x, 0.02f, -10.0f, 10.0f, "%.2f");
				ImGui::DragFloat("y",&m_TextureScale.y, 0.02f, -10.0f, 10.0f, "%.2f");
			}
			ImGui::End();
		}
	}

private:
	ShaderLibrary m_ShaderLibrary;

	Ref<VertexArray> m_VertexArray;

	Ref<Texture2D> m_Texture2D;
	Ref<Texture2D> m_TransparentTexture2D;

	Timestep m_Timestep;

	OrthographicCameraController m_CameraController;


	glm::vec3 m_TexturePosition = glm::vec3(0.0f);
	float m_TextureMoveSpeed = 0.5f;

	glm::vec3 m_TextureScale = glm::vec3(1.6f, 0.5f, 1.0f);

};




class Sandbox : public Application {
public:
	Sandbox(){
		EN_TRACE("Sandbox Created");
		PushLayer(new ExampleLayer());
	}

	~Sandbox(){
		EN_TRACE("Sandbox Deleted");
	}

	// void Run() override {
	// 	EN_TRACE("Sandbox Run");
	// }
};


Enik::Application* Enik::CreateApplication(){
	return new Sandbox();
}
