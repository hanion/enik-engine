#pragma once
#include "../graphics_context.h"

namespace Enik {
class OpenGLContext : public GraphicsContext {
public:
	OpenGLContext(GLFWwindow* windowHandle);
	~OpenGLContext();

	virtual void Init() override;
	virtual void SwapBuffers() override;

private:
	GLFWwindow* m_WindowHandle;
};

}