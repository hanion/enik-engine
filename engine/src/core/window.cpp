#include "window.h"

#include "events/application_event.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "renderer/opengl/opengl_context.h"

namespace Enik {

static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* desc) {
	EN_CORE_ERROR("GLFW Error ({0}) : {1}", error, desc);
}

Window::Window(const WindowProperties& properties) {
	Init(properties);
}

Window::~Window() {
	Shutdown();
}

void Window::Init(const WindowProperties& properties){
	EN_PROFILE_SCOPE;

	m_Data.Title = properties.Title;
	m_Data.Width = properties.Width;
	m_Data.Height = properties.Height;

	EN_CORE_INFO("Creating window {0} ({1}, {2}).", properties.Title, properties.Width, properties.Height);

	if (!s_GLFWInitialized) {
		EN_PROFILE_SECTION("glfwInit");

		int success = glfwInit();
		EN_CORE_ASSERT(success, "Could not initialize GLFW!");

		glfwSetErrorCallback(GLFWErrorCallback);

		s_GLFWInitialized = true;
	}
	{
		EN_PROFILE_SECTION("glfwCreateWindow");

		m_Window = glfwCreateWindow((int)properties.Width, (int)properties.Height, m_Data.Title.c_str(), nullptr, nullptr);
	}

	m_Context = CreateScope<OpenGLContext>(m_Window);
	m_Context->Init();

	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVsync(true);



	// set GLFW callbacks
	glfwSetWindowSizeCallback(m_Window,
		[](GLFWwindow* window, int width, int height){
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		}
	);

	glfwSetWindowCloseCallback(m_Window,
		[](GLFWwindow* window){
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		}
	);

	glfwSetKeyCallback(m_Window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods){
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action){
				case GLFW_PRESS: {
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}

				case GLFW_REPEAT:{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}

			}
		}
	);


	glfwSetCharCallback(m_Window,
		[](GLFWwindow* window, unsigned int keycode){
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		}
	);


	glfwSetMouseButtonCallback(m_Window,
		[](GLFWwindow* window, int button, int action, int mods){
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		}
	);


	glfwSetScrollCallback(m_Window,
		[](GLFWwindow* window, double xOffset, double yOffset){
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		}
	);

	glfwSetCursorPosCallback(m_Window,
		[](GLFWwindow* window, double xPos, double yPos){
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		}
	);



}

void Window::Shutdown() {
	EN_PROFILE_SCOPE;

	glfwDestroyWindow(m_Window);
//  	glfwTerminate();
}

void Window::OnUpdate() {
	EN_PROFILE_SCOPE;

	glfwPollEvents();
	m_Context->SwapBuffers();
}

void Window::SetVsync(bool enabled) {
	if (enabled) {
		glfwSwapInterval(1);
	}
	else {
		glfwSwapInterval(0);
	}

	m_Data.VSync = enabled;
}

bool Window::IsVSync() const {
	return m_Data.VSync;
}

GLFWwindow* Window::GetNativeWindow() {
	return m_Window;
}

}