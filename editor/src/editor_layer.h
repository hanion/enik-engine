#pragma once
#include <Enik.h>

using namespace Enik;

class EditorLayer : public Layer {
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	virtual void OnAttach() override final;
	virtual void OnDetach() override final;

	virtual void OnUpdate(Timestep timestep) override final;
	virtual void OnEvent(Event& event) override final;
	virtual void OnImGuiRender() override final;

	void OnImGuiDockSpaceRender();

private:
	OrthographicCameraController m_CameraController;

	// Temporary
	Ref<Shader> m_Shader;
	Ref<Texture2D> m_Texture2D;

	Ref<FrameBuffer> m_FrameBuffer;

	Ref<Scene> m_ActiveScene;

	bool m_ViewportFocused = false;
	bool m_ViewportHovered = false;
	glm::vec2 m_ViewportSize = glm::vec2(0);

	Entity m_Tile;
	Entity m_CameraEntity;

	Timestep m_Timestep;

};