#include "menu.h"
#include "common.h"
#include "follow.h"
#include "globals.h"

namespace Enik {

void MainMenu::OnCreate() {
	if (GetGlobals() and GetGlobals()->dead) {
// 		DestroyEntity(FindEntityByName("player"));
// 		Entity cam = FindEntityByName("camera");
// 		cam.Get<Component::Transform>().LocalPosition = { 0.0f, 6.0f, 0.0f };
// 		Follow* f = ((Follow*)cam.GetScriptInstance());
// 		if (Entity e = FindEntityByName("start_point")) {
// 			f->m_TargetID = e.GetID();
// 		}
	}
}

void MainMenu::OnPressed(Entity button) {
	if (!button || !button.Has<Component::Tag>()) {
		return;
	}

	const auto& tag = button.GetTag();
	if (tag == "start") {
		ChangeScene(ARENA_SCENE);
	} else if (tag == "quit") {
		CloseApplication();
	} else if (tag == "return_to_menu") {
		ChangeScene(MENU_SCENE);
	}
}

}
