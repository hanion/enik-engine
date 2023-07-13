#pragma once
#include <base.h>
#include "../include/pch.h"
#include "events/event.h"
#include "../../../include/log.h"

#include <GLFW/glfw3.h>


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

	//static Window* Create(const WindowProperties& properties = WindowProperties());

private:
	virtual void Init(const WindowProperties& properties);
	virtual void Shutdown();
private:
	GLFWwindow* m_Window;

	struct WindowData
	{
		std::string Title;
		unsigned int Width, Height;
		bool VSync;

		EventCallbackFn EventCallback;
	};

	WindowData m_Data;

	// static Scope<Window> Create(const WindowProperties& properties = WindowProperties());
	// static std::unique_ptr<Window>* Create(const WindowProperties& properties = WindowProperties());
	
	
	
};

}