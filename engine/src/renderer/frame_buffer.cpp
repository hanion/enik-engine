#include <pch.h>
#include "frame_buffer.h"
#include "renderer/renderer.h"
#include "renderer/opengl/opengl_frame_buffer.h"

namespace Enik {


Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec) {
	switch (Renderer::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLFrameBuffer>(spec);

		case RendererAPI::API::None:
			EN_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;

		default:
			EN_CORE_ASSERT(false, "Unknown RendererAPI");
			return nullptr;
	}
}

}