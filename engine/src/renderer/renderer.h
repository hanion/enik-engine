#pragma once
#include "render_command.h"
#include "orthographic_camera.h"
#include "opengl/shader.h"


namespace Enik {

class Renderer {
public:
	static void BeginScene(OrthographicCamera& camera);
	static void EndScene();
	
	static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

private:
	struct SceneData {
		glm::mat4 ViewProjectionMatrix;
	};

	static SceneData* m_SceneData;
	
};

}