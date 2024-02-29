#pragma once
#include "renderer_api.h"

namespace Enik {

class RenderCommand {
public:
	inline static void Init() {
		GetAPI()->Init();
	}

	inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		GetAPI()->SetViewport(x, y, width, height);
	}

	inline static void SetClearColor(const glm::vec4& color) {
		GetAPI()->SetClearColor(color);
	}

	inline static void Clear() {
		GetAPI()->Clear();
	}

	inline static void DrawIndexed(const Ref<VertexArray>& vertex_array, uint32_t index_count = 0) {
		GetAPI()->DrawIndexed(vertex_array, index_count);
	}
	inline static void DrawLine(const Ref<VertexArray>& vertex_array, uint32_t vertex_count = 0) {
		GetAPI()->DrawLine(vertex_array, vertex_count);
	}


	static RendererAPI* GetAPI();

private:
	static Scope<RendererAPI> s_RendererAPI;
};

}