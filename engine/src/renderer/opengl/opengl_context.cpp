#include "opengl_context.h"

#include <Enik.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <pch.h>

namespace Enik {

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle) {
	EN_CORE_ASSERT(windowHandle, "window handle is null !");
}

OpenGLContext::~OpenGLContext() {
}

void OpenGLContext::Init() {
	glfwMakeContextCurrent(m_WindowHandle);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	EN_CORE_ASSERT(status, "Failed to initialize glad!");
}
void OpenGLContext::SwapBuffers() {
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