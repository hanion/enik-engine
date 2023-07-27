#pragma once
#include "../renderer_api.h"

namespace Enik {

class OpenGLRendererAPI : public RendererAPI {
public:
	virtual void SetClearColor(const glm::vec4& color) override final;
	virtual void Clear() override final;

	virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override final;
};


}