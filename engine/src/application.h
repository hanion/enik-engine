#pragma once

#include "event.h"
#include <log.h>

namespace Enik {

class Application {

public:
    Application();
    virtual ~Application();

	virtual void Run();

};

// To be defined in CLIENT
Application* CreateApplication();

}