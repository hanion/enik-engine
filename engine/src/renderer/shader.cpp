#include <pch.h>
#include "shader.h"
#include "renderer.h"
#include "opengl/opengl_shader.h"

namespace Enik {


Shader* Shader::Create(const std::string& vertexSource, const std::string& fragmentSource) {
    switch (Renderer::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return new OpenGLShader(vertexSource, fragmentSource);

		case RendererAPI::API::None:
			EN_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;

		default:
			EN_CORE_ASSERT(false, "Unknown RendererAPI");
			return nullptr;
	}
}

}