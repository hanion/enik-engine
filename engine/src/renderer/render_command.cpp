#include <pch.h>
#include "render_command.h"
#include "opengl/opengl_renderer_api.h"


namespace Enik {

RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();

}