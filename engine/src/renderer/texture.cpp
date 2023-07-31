#include "texture.h"
#include "renderer.h"
#include "opengl/opengl_texture.h"


namespace Enik {

Ref<Texture2D> Enik::Texture2D::Create(const std::string& path) {
	switch (Renderer::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(path);

		case RendererAPI::API::None:
			EN_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;

		default:
			EN_CORE_ASSERT(false, "Unknown RendererAPI");
			return nullptr;
	}
}

}