#include "sandbox2D.h"

#include "renderer/opengl/opengl_shader.h"



Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1.6f/0.75f, true) {

}

void Sandbox2D::OnAttach() {
	EN_PROFILE_SCOPE;

}

void Sandbox2D::OnDetach() {
	EN_PROFILE_SCOPE;


}

void Sandbox2D::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	m_Timestep = timestep;

	m_CameraController.OnUpdate(m_Timestep);

	Renderer2D::ResetStats();

	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	RenderCommand::Clear();

	Renderer2D::BeginScene(m_CameraController.GetCamera());


	Renderer2D::EndScene();
}

void Sandbox2D::OnEvent(Event& event) {
	m_CameraController.OnEvent(event, true); // ? on sandbox, there is no ui, so we do not care about any event blocking
}

void Sandbox2D::OnImGuiRender() {
	/*ShowPerformance*/ {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		window_flags |=  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		window_flags |=  ImGuiWindowFlags_NoDecoration;

		ImGui::SetNextWindowBgAlpha(0.65f);
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y), ImGuiCond_Appearing);
		if (ImGui::Begin("Performance", nullptr, window_flags))
		{
			ImGui::Text("Performance");
			ImGui::Text("deltaTime = %.2fms", m_Timestep.GetMilliseconds());
			ImGui::Text("FPS = %.0f", (1.0f/m_Timestep.GetSeconds()));
		}
		ImGui::End();
	}
	/*ShowRenderer2DStats*/ {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		window_flags |=  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		window_flags |=  ImGuiWindowFlags_NoDecoration;

		ImGui::SetNextWindowBgAlpha(0.65f);
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y + 70), ImGuiCond_Appearing);
		if (ImGui::Begin("Rednerer2D Stats", nullptr, window_flags))
		{
			auto stats = Renderer2D::GetStats();

			ImGui::Text("Renderer2D Stats");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quad Count: %d", stats.QuadCount);
			ImGui::Text("Total Vertex Count: %d", stats.GetTotalVertexCount());
			ImGui::Text("Total Index  Count: %d", stats.GetTotalIndexCount());
		}
		ImGui::End();
	}
}
