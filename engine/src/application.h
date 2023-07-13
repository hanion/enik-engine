#pragma once

#include "event.h"
#include "window.h"
#include <log.h>

namespace Enik {

class Application {

public:
    Application();
    virtual ~Application();

	virtual void Run();


private:

    Window* m_Window;
    bool m_Running = true;

};

// To be defined in CLIENT
Application* CreateApplication();

}