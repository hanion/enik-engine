#include <Enik.h>
#include "core/entry_point.h"

#include "runtime.h"

using namespace Enik;

class RuntimeApp : public Application {
public:
	RuntimeApp() : Application(PROJECT_TITLE) {
		EN_TRACE("Runtime Created");
		PushLayer(new RuntimeLayer());
	}

	~RuntimeApp(){
		EN_TRACE("Runtime Deleted");
	}
};


Enik::Application* Enik::CreateApplication(){
	return new RuntimeApp();
}
