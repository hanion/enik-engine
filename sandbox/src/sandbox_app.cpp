#include <Enik.h>

using namespace Enik;

class ExampleLayer : public Layer {
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {
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
	

	void OnUpdate(Timestep timestep) override {
		m_Timestep = timestep;
		float deltaTime = m_Timestep.GetSeconds();

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


		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		// RenderCommand::SetClearColor({glm::abs(m_CameraPosition.x), glm::abs(m_CameraPosition.y), glm::abs(m_CameraPosition.z), 1.0f});
		RenderCommand::Clear();

		Renderer::BeginScene(m_Camera);		
		Renderer::Submit(m_Shader, m_VertexArray);

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
				ImGui::SliderFloat("X",&m_CameraPosition.x,-1.5f,1.5f, "%.2f");
				ImGui::SliderFloat("Y",&m_CameraPosition.y,-1.5f,1.5f, "%.2f");
				ImGui::SliderFloat("Z",&m_CameraPosition.z,-1.5f,1.5f, "%.2f");

				ImGui::Spacing();

				ImGui::Text("Rotation");
				ImGui::SliderFloat("X°",&m_CameraRotation.x,-180.0f,180.0f, "%.2f");
				ImGui::SliderFloat("Y°",&m_CameraRotation.y,-180.0f,180.0f, "%.2f");
				ImGui::SliderFloat("Z°",&m_CameraRotation.z,-180.0f,180.0f, "%.2f");

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
	}

private:
	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<VertexArray> m_VertexArray;
	Timestep m_Timestep;
public:
	OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_CameraRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	float m_CameraMoveSpeed = 1.0f;
	float m_CameraRotationSpeed = 50.0f;


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
