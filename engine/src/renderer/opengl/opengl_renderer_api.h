#pragma once
#include "renderer/renderer_api.h"

namespace Enik {

class OpenGLRendererAPI : public RendererAPI {
public:
	virtual void Init() override final;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override final;

	virtual void SetClearColor(const glm::vec4& color) override final;
	virtual void Clear() override final;

	virtual void DrawIndexed(const Ref<VertexArray>& vertex_array, uint32_t index_count) override final;
	virtual void DrawLine(const Ref<VertexArray>& vertex_array, uint32_t vertex_count) override final;
};


}