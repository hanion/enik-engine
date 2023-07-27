#include <pch.h>
#include "vertex_array.h"
#include "renderer.h"
#include "opengl/opengl_vertex_array.h"

namespace Enik {

VertexArray* VertexArray::Create() {
	switch (Renderer::GetAPI()) {
		case RendererAPI::OpenGL:
			return new OpenGLVertexArray();

		case RendererAPI::None:
			EN_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;

		default:
			EN_CORE_ASSERT(false, "Unknown RendererAPI");
			return nullptr;
	}
}


}