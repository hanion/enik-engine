#pragma once
#include <Enik.h>

using namespace Enik;

class Sandbox2D : public Layer {
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override final;
	virtual void OnDetach() override final;

	virtual void OnUpdate(Timestep timestep) override final;
	virtual void OnEvent(Event& event) override final;
	virtual void OnImGuiRender() override final;

private:
	OrthographicCameraController m_CameraController;

	// Temporary
	Ref<Shader> m_Shader;
	Ref<Texture2D> m_Texture2D;
	Ref<VertexArray> m_VertexArray;

	Timestep m_Timestep;

};