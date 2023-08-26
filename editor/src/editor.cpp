#include <Enik.h>

#include <glm/gtc/matrix_transform.hpp>

#include "core/entry_point.h"
#include "editor_layer.h"
#include "renderer/opengl/opengl_shader.h"

namespace Enik {

class Editor : public Application {
public:
	Editor() : Application("eeditor") {
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