#include "sandbox2D.h"

#include "renderer/opengl/opengl_shader.h"



Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1.6f/0.75f, true) {
	
}

void Sandbox2D::OnAttach() {
	m_Texture2D = Texture2D::Create(FULL_PATH("assets/textures/checkerboard.png"));
}

void Sandbox2D::OnDetach() {

}

void Sandbox2D::OnUpdate(Timestep timestep) {
	m_Timestep = timestep;

	m_CameraController.OnUpdate(m_Timestep);

	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	RenderCommand::Clear();

	Renderer2D::BeginScene(m_CameraController.GetCamera());

	Renderer2D::DrawQuad(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(0.2f, 0.98f, 0.2f, 1.0f));
	Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec2(100.0f, 100.0f), m_Texture2D, glm::vec4(0.2f,0.4f,0.4f,0.5f));

	Renderer2D::EndScene();
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
