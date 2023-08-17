#include "editor_layer.h"

#include "renderer/opengl/opengl_shader.h"
#include <glm/gtc/matrix_transform.hpp>



EditorLayer::EditorLayer()
	: Layer("EditorLayer"), m_CameraController(1.6f/0.75f, true) {
	
}

void EditorLayer::OnAttach() {
	EN_PROFILE_SCOPE;

	m_Texture2D = Texture2D::Create(FULL_PATH("assets/textures/checkerboard.png"));

	Ref<Texture2D> tileset = Texture2D::Create(FULL_PATH("assets/textures/tiles.png"));
	Ref<SubTexture2D> subTexture = SubTexture2D::CreateFromTileIndex(tileset, glm::vec2(18), glm::vec2(0,8), glm::vec2(2));

	FrameBufferSpecification spec;
	m_FrameBuffer = FrameBuffer::Create(spec);


	m_ActiveScene = CreateRef<Scene>();

	/* Creating Tile */{
		m_Tile = m_ActiveScene->CreateEntity();
		m_Tile.Add<Component::SpriteRenderer>().SubTexture = subTexture;
		m_Tile.Get<Component::Transform>().Position = glm::vec3(0.0f, 0.0f, 0.9f);
	}

	/* Creating Background Entity */{
		Entity backgroundEntity = m_ActiveScene->CreateEntity("Background");

		Component::Transform& trans = backgroundEntity.Get<Component::Transform>();
		trans.Position.z = -0.9f;
		trans.Scale = glm::vec2(200.0f);
		
		Component::SpriteRenderer& sprite = backgroundEntity.Add<Component::SpriteRenderer>();
		sprite.Texture = m_Texture2D;
		sprite.Color = glm::vec4(0.2f,0.4f,0.4f,0.5f);
		sprite.TileScale = 100.0f;

	}

	/* Create Camera Entity */{
		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.Add<Component::Camera>();
	}

}

void EditorLayer::OnDetach() {
	EN_PROFILE_SCOPE;


}

void EditorLayer::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	m_Timestep = timestep;

	m_CameraController.OnUpdate(m_Timestep);

	Renderer2D::ResetStats();

	m_FrameBuffer->Bind();

	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	RenderCommand::Clear();


#if 0 
	{
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
#endif


	
	m_Tile.Get<Component::Transform>().Rotation += glm::radians(15.0f) * timestep.GetSeconds();
	m_CameraEntity.Get<Component::Transform>().Rotation += glm::radians(15.0f) * timestep.GetSeconds();

	m_ActiveScene->OnUpdate(m_Timestep);

	m_FrameBuffer->Unbind();
}

void EditorLayer::OnEvent(Event& event) {
	m_CameraController.OnEvent(event);
}

void EditorLayer::OnImGuiRender() {
	/*DockSpace*/ {
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		window_flags |= ImGuiWindowFlags_NoBackground;


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::Begin("DockSpace", nullptr, window_flags);

		ImGui::PopStyleVar(3);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("DockSpaceID");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) {
					Application::Get().Close();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}


		OnImGuiDockSpaceRender();

		ImGui::End();
	}
}

void EditorLayer::OnImGuiDockSpaceRender() {
	EN_PROFILE_SCOPE;


	/*Viewport*/ {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
		ImGui::Begin("Viewport", nullptr, window_flags);
		ImGui::PopStyleVar(1);

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
		
		
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (((viewportSize.x != m_ViewportSize.x) or (viewportSize.y != m_ViewportSize.y)) and (viewportSize.x > 0 and viewportSize.y > 0)){
			m_ViewportSize.x = viewportSize.x;
			m_ViewportSize.y = viewportSize.y;

			m_FrameBuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(textureID)), 
			ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0,1), ImVec2(1,0));
		
		ImGui::End();
	}

	/*ShowPerformance*/ {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		window_flags |=  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		window_flags |=  ImGuiWindowFlags_NoDecoration;
		
		ImGui::SetNextWindowBgAlpha(0.65f);
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + 20, ImGui::GetWindowPos().y + 50), ImGuiCond_Appearing);
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
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + 20,ImGui::GetWindowPos().y + 120), ImGuiCond_Appearing);
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
