#pragma once
#include "renderer/renderer_api.h"

namespace Enik {

class OpenGLRendererAPI : public RendererAPI {
public:
	virtual void Init() override final;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override final;

	virtual void SetClearColor(const glm::vec4& color) override final;
	virtual void Clear() override final;

	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override final;
};


}