#include <pch.h>
#include "renderer_api.h"


namespace Enik {

RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
RendererAPI::API RendererAPI::GetAPI() { return RendererAPI::s_API; }

}
