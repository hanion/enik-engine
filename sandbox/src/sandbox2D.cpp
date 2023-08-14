#include "sandbox2D.h"

#include "renderer/opengl/opengl_shader.h"



Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1.6f/0.75f, true) {
	
}

void Sandbox2D::OnAttach() {
	EN_PROFILE_SCOPE;

	m_Texture2D = Texture2D::Create(FULL_PATH("assets/textures/checkerboard.png"));

	Ref<Texture2D> tileset = Texture2D::Create(FULL_PATH("assets/textures/tiles.png"));
	Ref<SubTexture2D> subTexture = SubTexture2D::CreateFromTileIndex(tileset, glm::vec2(18), glm::vec2(0,8), glm::vec2(2));
	m_Tile.subTexture = subTexture;
	m_Tile.position.z = 0.5f;
	
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

	if (false) {
		EN_PROFILE_SECTION("Squares");

		static const int count = 99;
		static const float spacing = 0.25f;
		static const glm::vec2 scale = glm::vec2(0.2f);
		static const float offset = ( ((count + (count%2)) * spacing) / 2.0f + spacing/2.0f);

		static const glm::vec4 darkColor  = glm::vec4(0.1f, 0.1f, 0.1f, 0.6f);
		static const glm::vec4 lightColor = glm::vec4(0.4f, 0.4f, 0.4f, 0.6f);

		QuadProperties square;
		square.scale = scale;
		square.position.z = -0.2f;

		for (int i = 0; i < count; i++){
			for (int j = 0; j < count; j++) {
				if (((i % 2 == 0) and (j % 2 == 0)) or ((i % 2 != 0) and (j % 2 != 0))) {
					square.color = darkColor;
				}
				else {
					square.color = lightColor;
				}

				square.position.x = j*spacing - offset;
				square.position.y = i*spacing - offset;
				
				Renderer2D::DrawQuad(square);
			}
			
		}
	}

	QuadProperties checkerboard;
	checkerboard.position.z = -0.9f;
	checkerboard.scale = glm::vec2(200.0f);
	checkerboard.texture = m_Texture2D;
	checkerboard.color = glm::vec4(0.2f,0.4f,0.4f,0.5f);
	checkerboard.tileScale = 100.0f;
	checkerboard.rotation = 0.0f;
	Renderer2D::DrawQuad(checkerboard);


	m_Tile.rotation += glm::radians(45.0f) * timestep.GetSeconds();
	Renderer2D::DrawQuad(m_Tile);

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
