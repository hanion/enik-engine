#pragma once
#include <base.h>
#include <pch.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "events/event.h"
#include "log.h"
#include "renderer/graphics_context.h"

namespace Enik {

struct WindowProperties {
	std::string Title;
	unsigned int Width;
	unsigned int Height;

	WindowProperties(const std::string& title = "eengine",
					 unsigned int width = 640,
					 unsigned int height = 360)
		: Title(title), Width(width), Height(height) {
	}
};

class Window {
public:
	using EventCallbackFn = std::function<void(Event&)>;

	Window(const WindowProperties& properties);
	~Window();

	void OnUpdate();

	inline unsigned int GetWidth() const { return m_Data.Width; }
	inline unsigned int GetHeight() const { return m_Data.Height; }

	inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

	void SetVsync(bool enabled);
	bool IsVSync() const;

	GLFWwindow* GetNativeWindow();

private:
	virtual void Init(const WindowProperties& properties);
	virtual void Shutdown();

private:
	GLFWwindow* m_Window;
	Scope<GraphicsContext> m_Context;

	struct WindowData {
		std::string Title;
		unsigned int Width, Height;
		bool VSync;

		EventCallbackFn EventCallback;
	};

	WindowData m_Data;

};

}