#include <Enik.h>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/opengl/opengl_shader.h"

using namespace Enik;

class ExampleLayer : public Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.75f, 0.75f) {
		m_VertexArray.reset(VertexArray::Create());

		float vertices[4 * 9] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
		};


		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		
		
		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"}
		};
		
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
		
		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		/*Create shader sources*/{
			std::string vertexSource = R"(
				#version 330 core

				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Color;

				uniform mat4 u_ViewProjection;
				uniform mat4 u_Transform;
				
				out vec3 v_Position;
				out vec4 v_Color;

				void main() {
					v_Position = a_Position;
					v_Color = a_Color;
					gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
				}
			)";

			std::string fragmentSource = R"(
				#version 330 core

				layout(location = 0) out vec4 color;
				in vec3 v_Position;
				in vec4 v_Color;

				void main() {
					color = v_Color;
				}
			)";

			std::string TextureShaderVertexSource = R"(
				#version 330 core

				layout(location = 0) in vec3 a_Position;
				layout(location = 2) in vec2 a_TexCoord;

				uniform mat4 u_ViewProjection;
				uniform mat4 u_Transform;
				
				out vec2 v_TexCoord;


				void main() {
					v_TexCoord = a_TexCoord;
					gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
				}
			)";

			std::string TextureShaderFragmentSource = R"(
				#version 330 core

				layout(location = 0) out vec4 color;
				in vec2 v_TexCoord;
				uniform sampler2D u_Texture;

				void main() {
					//color = vec4(v_TexCoord, 0.0, 1.0);
					color = texture(u_Texture, v_TexCoord);
				}
			)";

			m_Shader.reset(Shader::Create(vertexSource, fragmentSource));
			
			m_TextureShader.reset(Shader::Create(TextureShaderVertexSource, TextureShaderFragmentSource));
			std::dynamic_pointer_cast<OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
			m_Texture2D = Texture2D::Create("/home/han/dev/enik-engine/sandbox/assets/checkerboard.png");
			m_TransparentTexture2D = Texture2D::Create("/home/han/dev/enik-engine/sandbox/assets/tablordia_banner.png");
		}
	}
	
	void ControlCamera(float& deltaTime) {
		if(Input::IsKeyPressed(Key::A)){
			m_CameraPosition -= glm::vec3(1, 0, 0) * m_CameraMoveSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(Key::D)) {
			m_CameraPosition += glm::vec3(1, 0, 0) * m_CameraMoveSpeed * deltaTime;
		}

		if(Input::IsKeyPressed(Key::W)){
			m_CameraPosition += glm::vec3(0, 1, 0) * m_CameraMoveSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(Key::S)) {
			m_CameraPosition -= glm::vec3(0, 1, 0) * m_CameraMoveSpeed * deltaTime;
		}

		if(Input::IsKeyPressed(Key::Q)){
			m_CameraRotation += glm::vec3(0, 0, 1) * m_CameraRotationSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(Key::E)) {
			m_CameraRotation -= glm::vec3(0, 0, 1) * m_CameraRotationSpeed * deltaTime;
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
	}


	void OnUpdate(Timestep timestep) override {
		m_Timestep = timestep;
		float deltaTime = m_Timestep.GetSeconds();

		ControlCamera(deltaTime);		



		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		RenderCommand::Clear();

		Renderer::BeginScene(m_Camera);

		/*Create mini squares*/ {
			static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
			for (size_t i = 0; i < 5; i++) {
				glm::vec3 pos = glm::vec3(i * 0.2f - (2.0f*0.2f), -0.6f, 0.0f);
				glm::mat4 miniTransform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Renderer::Submit(m_Shader, m_VertexArray, miniTransform);
			}
		}


		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_TexturePosition);
		
		m_Texture2D->Bind();
		Renderer::Submit(m_TextureShader, m_VertexArray, glm::mat4(1.0f));
		
		m_TransparentTexture2D->Bind();
		Renderer::Submit(m_TextureShader, m_VertexArray, glm::scale(transform, m_TextureScale));

		Renderer::EndScene(); 
	}

	void OnEvent(Event& event) override {

	}

	virtual void OnImGuiRender() override {
		/*ShowCameraControls*/ {
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			if (ImGui::Begin("Camera", NULL, window_flags)) {

				ImGui::Text("Translation");
				ImGui::DragFloat("X",&m_CameraPosition.x, 0.02f, -10.0f,10.0f, "%.2f");
				ImGui::DragFloat("Y",&m_CameraPosition.y, 0.02f, -10.0f,10.0f, "%.2f");
				ImGui::DragFloat("Z",&m_CameraPosition.z, 0.02f, -10.0f,10.0f, "%.2f");

				ImGui::Spacing();

				ImGui::Text("Rotation");
				ImGui::DragFloat("X°",&m_CameraRotation.x, 0.2f, -180.0f,180.0f, "%.2f");
				ImGui::DragFloat("Y°",&m_CameraRotation.y, 0.2f, -180.0f,180.0f, "%.2f");
				ImGui::DragFloat("Z°",&m_CameraRotation.z, 0.2f, -180.0f,180.0f, "%.2f");

			}
			ImGui::End();
		}

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
	Ref<Shader> m_Shader;
	Ref<VertexArray> m_VertexArray;

	Ref<Shader> m_TextureShader;
	Ref<Texture2D> m_Texture2D;
	Ref<Texture2D> m_TransparentTexture2D;

	Timestep m_Timestep;

	OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_CameraRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	float m_CameraMoveSpeed = 1.0f;
	float m_CameraRotationSpeed = 50.0f;

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
