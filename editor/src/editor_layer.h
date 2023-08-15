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

	bool m_ViewportFocused = false;
	bool m_ViewportHovered = false;
	glm::vec2 m_ViewportSize = glm::vec2(0);

	QuadProperties m_Tile;

	Timestep m_Timestep;

};