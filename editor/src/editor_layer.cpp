#include <pch.h>
#include "editor_layer.h"

#include "renderer/opengl/opengl_shader.h"
#include <glm/gtc/matrix_transform.hpp>

#include "scene/scene_serializer.h"



EditorLayer::EditorLayer()
	: Layer("EditorLayer") {
	
}

void EditorLayer::OnAttach() {
	EN_PROFILE_SCOPE;

	FrameBufferSpecification spec;
	m_FrameBuffer = FrameBuffer::Create(spec);


	m_ActiveScene = CreateRef<Scene>();

	/* Creating Tile */{
		Ref<Texture2D> tileset = Texture2D::Create(FULL_PATH("assets/textures/tiles.png"));
		Ref<SubTexture2D> subTexture = SubTexture2D::CreateFromTileIndex(tileset, glm::vec2(18), glm::vec2(0,8), glm::vec2(2));
		
		m_Tile = m_ActiveScene->CreateEntity("Grass Tile");
		m_Tile.Add<Component::SpriteRenderer>().SubTexture = subTexture;
		m_Tile.Get<Component::Transform>().Position = glm::vec3(0.0f, 0.0f, 0.9f);
	}

	/* Creating Background Entity */{
		Ref<Texture2D> backgroundTexture = Texture2D::Create(FULL_PATH("assets/textures/checkerboard.png"));

		Entity backgroundEntity = m_ActiveScene->CreateEntity("Background");

		Component::Transform& trans = backgroundEntity.Get<Component::Transform>();
		trans.Position.z = -0.9f;
		trans.Scale = glm::vec2(200.0f);
		
		Component::SpriteRenderer& sprite = backgroundEntity.Add<Component::SpriteRenderer>();
		sprite.Texture = backgroundTexture;
		sprite.Color = glm::vec4(0.2f,0.4f,0.4f,0.5f);
		sprite.TileScale = 100.0f;

	}

	/* Create Camera Entity */{
		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.Add<Component::Camera>();
	}

	/* Create a test native script */ {
		class CameraContoller : public ScriptableEntity {
		public:
			CameraContoller() 
				: m_Transform(nullptr) {}

			virtual void OnCreate() override final {
				m_Transform = &Get<Component::Transform>();
			}
			
			virtual void OnUpdate(Timestep ts) override final {
				if (Input::IsKeyPressed(Key::A)) {
					m_Transform->Position.x -= m_Speed * ts;
				}
				if (Input::IsKeyPressed(Key::D)) {
					m_Transform->Position.x += m_Speed * ts;
				}
				
				if (Input::IsKeyPressed(Key::W)) {
					m_Transform->Position.y += m_Speed * ts;
				}
				if (Input::IsKeyPressed(Key::S)) {
					m_Transform->Position.y -= m_Speed * ts;
				}
			}
		private:
			const glm::vec3 start_pos = glm::vec3(0.0f);
			float m_Speed = 5.0f;
			Component::Transform* m_Transform;
		};

		//m_CameraEntity.AddScript<CameraContoller>();
		m_CameraEntity.Add<Component::NativeScript>().Bind<CameraContoller>();

	}

	/* Create a tile rotator native script */ {
		class TileRotator : public ScriptableEntity {
		public:
			virtual void OnCreate() override final {
				m_Transform = &Get<Component::Transform>();
			}
			
			virtual void OnUpdate(Timestep ts) override final {
				m_Transform->Rotation += glm::radians(15.0f) * ts.GetSeconds();
			}
		private:
			float m_Speed = 5.0f;
			Component::Transform* m_Transform = nullptr;
		};

		m_Tile.Add<Component::NativeScript>().Bind<TileRotator>();
	}

	m_SceneTreePanel.SetContext(m_ActiveScene);
	m_InspectorPanel.SetContext(m_ActiveScene, &m_SceneTreePanel);

}

void EditorLayer::OnDetach() {
	EN_PROFILE_SCOPE;


}

void EditorLayer::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	m_Timestep = timestep;

	FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
	if (m_ViewportSize.x > 0.0f and m_ViewportSize.y > 0.0f and (spec.Width != m_ViewportSize.x or spec.Height != m_ViewportSize.y)) {
		m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	Renderer2D::ResetStats();

	m_FrameBuffer->Bind();

	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	RenderCommand::Clear();

	m_ActiveScene->OnUpdate(m_Timestep);

	m_FrameBuffer->Unbind();
}

void EditorLayer::OnEvent(Event& event) {
	
}

void EditorLayer::OnImGuiRender() {
	/*DockSpace*/ {
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
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
				if (ImGui::MenuItem("New")) {
					// TODO: prompt dialog confirm
					CreateNewScene();
				}
				
				if (ImGui::MenuItem("Open File")) {
					m_ShowFileDialogAs = DialogType::OPEN_FILE;
					m_IsDialogOpen = true;
				}
				
				if (ImGui::MenuItem("Save")) {
					if (m_ActiveScenePath.empty()) {
						m_ShowFileDialogAs = DialogType::SAVE_FILE;
						m_IsDialogOpen = true;
					}
					else {
						SaveScene();
					}
				}
				if (ImGui::MenuItem("Save As")) {
					m_ShowFileDialogAs = DialogType::SAVE_FILE;
					m_IsDialogOpen = true;
				}
				
				if (ImGui::MenuItem("Exit")) {
					Application::Get().Close();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {
				ImGui::Checkbox("Show Performance",    &m_ShowPerformance  );
				ImGui::Checkbox("Show Renderer Stats", &m_ShowRendererStats);

				ImGui::EndMenu();
			}

			
			ImGui::EndMenuBar();
		}


		OnImGuiDockSpaceRender();

		ImGui::End();
	}

	if (DialogFile::Show(m_IsDialogOpen, m_ShowFileDialogAs) == DialogResult::ACCEPT) {
		if (m_ShowFileDialogAs == DialogType::OPEN_FILE) {
			CreateNewScene();
			LoadScene(DialogFile::GetSelectedPath());
		}
		else if (m_ShowFileDialogAs == DialogType::SAVE_FILE) {
			m_ActiveScenePath = DialogFile::GetSelectedPath();
			SaveScene();
		}
	}

}

void EditorLayer::OnImGuiDockSpaceRender() {
	EN_PROFILE_SCOPE;

	m_SceneTreePanel.OnImGuiRender();
	m_InspectorPanel.OnImGuiRender();

	/*Viewport*/ {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
		ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f), ImGuiCond_FirstUseEver);
		ImGui::Begin("Viewport", nullptr, window_flags);
		ImGui::PopStyleVar(1);

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
		
		
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (((viewportSize.x != m_ViewportSize.x) or (viewportSize.y != m_ViewportSize.y)) and (viewportSize.x > 0 and viewportSize.y > 0)){
			m_ViewportSize.x = viewportSize.x;
			m_ViewportSize.y = viewportSize.y;

			m_FrameBuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(textureID)), 
			ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0,1), ImVec2(1,0));
		

		auto pos = ImGui::GetWindowPos();
		m_ViewportPosition.x = pos.x;
		m_ViewportPosition.y = pos.y;

		ImGui::End();
	}

	/* ShowPerformance */
	if (m_ShowPerformance) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		window_flags |=  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		window_flags |=  ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking;
		
		ImGui::SetNextWindowBgAlpha(0.65f);
		ImVec2 pos;
		pos.x = 20 + m_ViewportPosition.x;
		pos.y = 40 + m_ViewportPosition.y;

		ImGui::SetNextWindowPos(pos);
		if (ImGui::Begin("Performance", nullptr, window_flags))
		{
			ImGui::Text("Performance");
			ImGui::Text("deltaTime = %.2fms", m_Timestep.GetMilliseconds());
			ImGui::Text("FPS = %.0f", (1.0f/m_Timestep.GetSeconds()));
		}
		ImGui::End();
	}


	/* ShowRenderer2DStats */
	if (m_ShowRendererStats) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		window_flags |=  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
		window_flags |=  ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking;
		
		ImGui::SetNextWindowBgAlpha(0.65f);
		ImVec2 pos;
		pos.x = 20  + m_ViewportPosition.x;
		pos.y = 130 + m_ViewportPosition.y;

		ImGui::SetNextWindowPos(pos);
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


void EditorLayer::CreateNewScene() {
	m_ActiveScene = CreateRef<Scene>();
	m_SceneTreePanel.SetContext(m_ActiveScene);
	m_InspectorPanel.SetContext(m_ActiveScene, &m_SceneTreePanel);
}

void EditorLayer::LoadScene(const std::string& path) {
	m_ActiveScenePath = path;
	SceneSerializer serializer = SceneSerializer(m_ActiveScene);
	serializer.Deserialize(m_ActiveScenePath);
	m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
}

void EditorLayer::SaveScene() {
	SceneSerializer serializer = SceneSerializer(m_ActiveScene);
	serializer.Serialize(m_ActiveScenePath);
}
