#pragma once

#include "window.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "events/application_event.h"
#include <log.h>
#include "layers/layer_stack.h"

namespace Enik {

class Application {

public:
    Application();
    virtual ~Application();
	virtual void Run();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);

private:
    bool OnWindowClose(WindowCloseEvent& e);

    Window* m_Window;
    bool m_Running = true;
    LayerStack m_LayerStack;

};

// To be defined in CLIENT
Application* CreateApplication();

}