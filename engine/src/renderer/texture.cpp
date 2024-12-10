#include "texture.h"
#include "renderer.h"
#include "opengl/opengl_texture.h"


namespace Enik {
Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification, Buffer data) {
	switch (Renderer::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(specification, data);

		case RendererAPI::API::None:
			EN_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;

		default:
			EN_CORE_ASSERT(false, "Unknown RendererAPI");
			return nullptr;
	}
}


}