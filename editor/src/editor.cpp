#include <Enik.h>

#include "core/entry_point.h"
#include "editor_layer.h"

namespace Enik {

class Editor : public Application {
public:
	Editor() : Application("enik-engine") {
		EN_TRACE("Editor Created");
		PushLayer(new EditorLayer());
	}

	~Editor() {
		EN_TRACE("Editor Deleted");
	}
};

Application* CreateApplication() {
	return new Editor();
}

}