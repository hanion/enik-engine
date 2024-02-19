#include <pch.h>
#include "render_command.h"
#include "opengl/opengl_renderer_api.h"


namespace Enik {

Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<OpenGLRendererAPI>();
RendererAPI* RenderCommand::GetAPI() { return RenderCommand::s_RendererAPI.get(); }

}