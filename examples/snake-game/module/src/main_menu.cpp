#include "main_menu.h"
#include "game_manager.h"


namespace Enik {






void MainMenu::OnPressed(Entity button) {
	// CONSOLE_DEBUGS("wee {}", button.GetTag());
	std::string& tag = button.GetTag();


	if (tag == "play") {
		MoveCam(true);
		PlayGame(true);
	}
	else if (tag == "quit") {
		MoveCam(false);
		CloseApplication();
		// Application::Get().Close(); // TODO use this
		// throw std::runtime_error("closed the game, sorry for this.");
	}
	else if (tag == "back") {
		PlayGame(false);
		MoveCam(false);
	}
}

void MainMenu::OnUpdate(Timestep ts) {
	ButtonContainer::OnUpdate(ts);
}

void MainMenu::OnMouseButtonPressed(const MouseButtonPressedEvent& event) {
	ButtonContainer::OnMouseButtonPressed(event);
}
void MainMenu::OnMouseButtonReleased(const MouseButtonReleasedEvent& event) {
	ButtonContainer::OnMouseButtonReleased(event);
}

void MainMenu::OnKeyPressed(const KeyPressedEvent& event) {
	switch (event.GetKeyCode()) {
	case Key::Escape:
		PlayGame(false);
		MoveCam(false);
		break;

	default:
		break;
	}
}

void MainMenu::PlayGame(bool is_playing) {
	if (Entity game = FindEntityByUUID(m_Game)) {
		if (game.Has<Component::NativeScript>()) {
			GameManager* script = (GameManager*)game.GetScriptInstance();
			if (script) {
				script->ResetState();
				script->SetPlaying(is_playing);
			}
		}
	}
}

void MainMenu::MoveCam(bool to_game) {
	if (Entity cam = FindEntityByUUID(m_CameraUUID)) {
		if (to_game) {
			cam.Get<Component::Transform>().LocalPosition = { -0.5f, -30.5f, 0.0f };
		} else {
			cam.Get<Component::Transform>().LocalPosition = { 0.0f, 0.0f, 0.0f };
		}
	}
}

}