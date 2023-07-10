#pragma once

namespace Enik {

class Application {

public:
    Application();
    ~Application();

	void Run();

};

// To be defined in CLIENT
Application* CreateApplication();

}