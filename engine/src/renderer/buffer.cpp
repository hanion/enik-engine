#include <pch.h>
#include "buffer.h"
#include "renderer.h"
#include <log.h>

#include "opengl/opengl_buffer.h"

namespace Enik {

VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) {
	switch (Renderer::GetAPI()) {
		case RendererAPI::OpenGL:
			return new OpenGLVertexBuffer(vertices, size);

		case RendererAPI::None:
			EN_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;

		default:
			EN_CORE_ASSERT(false, "Unknown RendererAPI");
			return nullptr;
	}
}


IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count) {
	switch (Renderer::GetAPI()) {
		case RendererAPI::OpenGL:
			return new OpenGLIndexBuffer(indices, count);

		case RendererAPI::None:
			EN_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;

		default:
			EN_CORE_ASSERT(false, "Unknown RendererAPI");
			return nullptr;
	}
}

}