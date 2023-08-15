#include <Enik.h>
#include "core/entry_point.h"
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/opengl/opengl_shader.h"

#include "editor_layer.h"

namespace Enik {

class Editor : public Application {
public:
	Editor() : Application("eeditor") {
		EN_TRACE("Editor Created");
		PushLayer(new EditorLayer());
	}

	~Editor(){
		EN_TRACE("Editor Deleted");
	}
};


Application* CreateApplication(){
	return new Editor();
}

}