#include "opengl_context.h"

#include <Enik.h>
#include <pch.h>

namespace Enik {

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle) {
	EN_CORE_ASSERT(windowHandle, "window handle is null !");
}

OpenGLContext::~OpenGLContext() {
}

void OpenGLContext::Init() {
	EN_PROFILE_SCOPE;

	glfwMakeContextCurrent(m_WindowHandle);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	EN_CORE_ASSERT(status, "Failed to initialize glad!");

	EN_CORE_INFO("OpenGL Renderer: {0}",(const char*)glGetString(GL_RENDERER));
	EN_CORE_INFO("OpenGL Version: {0}",(const char*)glGetString(GL_VERSION));
}
void OpenGLContext::SwapBuffers() {
	EN_PROFILE_SCOPE;
	/*
	// renders a triangle in the middle
	glBegin(GL_TRIANGLES);
	glVertex3f(0.0f, 0.8f, 0.0f);   // Vertex 1 (top)
	glVertex3f(-0.5f, -0.1f, 0.0f); // Vertex 2 (bottom-left)
	glVertex3f(0.5f, -0.1f, 0.0f);  // Vertex 3 (bottom-right)
	glEnd();
	*/
	glfwSwapBuffers(m_WindowHandle);
}
}