#pragma once
#include "renderer_api.h"

namespace Enik {

class RenderCommand {
public:
	inline static void Init() {
		s_RendererAPI->Init();
	}

	inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		s_RendererAPI->SetViewport(x, y, width, height);
	}

	inline static void SetClearColor(const glm::vec4& color) {
		s_RendererAPI->SetClearColor(color);
	}

	inline static void Clear() {
		s_RendererAPI->Clear();
	}

	inline static void DrawIndexed(const Ref<VertexArray>& vertex_array, uint32_t index_count = 0) {
		s_RendererAPI->DrawIndexed(vertex_array, index_count);
	}
	inline static void DrawLine(const Ref<VertexArray>& vertex_array, uint32_t vertex_count = 0) {
		s_RendererAPI->DrawLine(vertex_array, vertex_count);
	}


private:
	static Scope<RendererAPI> s_RendererAPI;
};

}