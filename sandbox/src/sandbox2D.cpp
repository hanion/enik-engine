#include "sandbox2D.h"

#include "renderer/opengl/opengl_shader.h"



Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1.6f/0.75f, true) {
	
}

void Sandbox2D::OnAttach() {
	EN_PROFILE_SCOPE;

	m_Texture2D = Texture2D::Create(FULL_PATH("assets/textures/checkerboard.png"));
	m_Texture2D_2 = Texture2D::Create(FULL_PATH("assets/textures/tablordia_banner.png"));
}

void Sandbox2D::OnDetach() {
	EN_PROFILE_SCOPE;


}

void Sandbox2D::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	m_Timestep = timestep;

	m_CameraController.OnUpdate(m_Timestep);

	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	RenderCommand::Clear();

	Renderer2D::BeginScene(m_CameraController.GetCamera());

	// Renderer2D::DrawQuad(glm::vec2(-0.5f, -0.5f), glm::vec2(1.0f, 1.0f), glm::vec4(0.2f, 0.98f, 0.2f, 1.0f));
	Renderer2D::DrawQuad(glm::vec3(-100.0f, -100.0f, -0.5f), glm::vec2(200.0f, 200.0f), m_Texture2D, glm::vec4(0.2f,0.4f,0.4f,0.5f), 100.0f);
 
	if (false) {
		EN_PROFILE_SECTION("Squares");

		static const int count = 99;
		static const float spacing = 0.25f;
		static const glm::vec2 scale = glm::vec2(0.2f);
		static const glm::vec2 offset = glm::vec2((count+(count%2))*spacing*0.5f + 0.025f);//glm::vec2(count*0.25f);

		static const glm::vec4 darkColor  = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		static const glm::vec4 lightColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
		
		
		for (int i = 0; i < count; i++){
			for (int j = 0; j < count; j++) {
				glm::vec4 color;
				if (((i % 2 == 0) and (j % 2 == 0)) or ((i % 2 != 0) and (j % 2 != 0))) {
					color = darkColor;
				}
				else {
					color = lightColor;
				}
				Renderer2D::DrawQuad(offset - glm::vec2(j*spacing, i*spacing), scale, color);
				// Renderer2D::DrawQuad(offset - glm::vec2(j*0.2f, i*0.2f), glm::vec2(0.19f, 0.19f), glm::vec4(0.2f*j*0.1f, 0.2f, 0.1f*i*0.1f, 1.0f));
			}
			
		}
	}

	Renderer2D::DrawQuad(glm::vec3(-1.5f,  -0.5f,   0.3f), glm::vec2(3.0f, 1.0f), m_Texture2D_2, glm::vec4(1.0f));

	Renderer2D::EndScene();
}

void Sandbox2D::OnEvent(Event& event) {
	m_CameraController.OnEvent(event);
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
}
