#include "player.h"
#include "common.h"
#include "gui.h"
#include "globals.h"
#include <string>


namespace Enik {

void Player::DamageCharacter(float damage, UUID shooter) {
	if (!shooter) {
		return;
	}
	Entity sho = FindEntityByUUID(shooter);
	if (!sho || !sho.Has<Component::NativeScript>()) {
		return;
	}

	m_Health -= damage;
	auto gui = GetGUI();
	if (gui) {
		gui->SetHealth(m_Health);
	}
	
	Get<Component::AudioSources>().Play("hit");
	if (m_Health <= 0.0f) {
		RestartGame();
	}
}

void Player::OnKilledEnemy() {
	AddScore(10);
}

void Player::AddScore(int s) {
	if (auto globals = GetGlobals()) {
		SetScore(globals->score + s);
	}
}
void Player::SetScore(int s) {
	if (auto globals = GetGlobals()) {
		globals->score = s;
	}
	if (auto gui = GetGUI()) {
		gui->SetScore(s);
	}
}

void Player::OnCreate() {
	FindWeaponScript();
}

void Player::OnUpdate(Timestep ts) {
	if (Get<Component::Transform>().GlobalPosition.y < -25.0f) {
		RestartGame();
		return;
	}

	m_waiting_animation = GetGlobals()->waiting_animation;
	if (m_waiting_animation) {
		return;
	}

	// mouse world position
	Entity cam = FindEntityByUUID(m_CameraUUID);
	m_MouseWorldPosition = glm::vec3(cam.Get<Component::Camera>().Cam.GetWorldPosition(Input::GetMousePosition()), 0);
	m_MouseWorldPosition += cam.Get<Component::Transform>().LocalPosition;

	RotateWeapon(m_MouseWorldPosition);
}

void Player::OnFixedUpdate() {
	if (!Has<Component::Transform>()) {
		return;
	}

	if (m_waiting_animation) {
		return;
	}

// 	auto pos = Get<Component::Transform>().GlobalPosition;
// 	Raycast ray = {
// 		pos + glm::vec3(0,-1,0),
// 		pos + glm::vec3(0,-2,0),
// 		0
// 	};
// 	RaycastResult rr = CastRay(ray);
// 	if (rr.entity) {
// 		if (rr.entity.GetTag() == "ground") {
// 			m_IsGrounded = true;
// 		} else {
// 			CONSOLE_DEBUGS("rr {}", rr.entity.GetTag().c_str());
// 		}
// 	}

	glm::vec3 input = glm::vec3(0.0f);

	if (Input::IsKeyPressed(Key::A)) {
		input.x -= 1.0f;
	}
	if (Input::IsKeyPressed(Key::D)) {
		input.x += 1.0f;
	}
	if (input.x != 0.0f) {
		input = glm::normalize(input);
	}
	Move(input);

	bool jump_pressed = Input::IsKeyPressed(Key::Space);
	if (jump_pressed && !m_JumpPressedLastFrame) {
		Jump();
	}
	m_JumpPressedLastFrame = jump_pressed;
}


void Player::OnKeyPressed(const KeyPressedEvent& event) {
	if (event.IsRepeat()) {
		return;
	}

	// bool control = Input::IsKeyPressed(Key::LeftControl) or Input::IsKeyPressed(Key::RightControl);
	// bool shift   = Input::IsKeyPressed(Key::LeftShift)   or Input::IsKeyPressed(Key::RightShift);

	switch (event.GetKeyCode()) {
		// jump
		case Key::Space: {
			if (m_IsGrounded and !m_waiting_animation) {
				//Move({0, m_JumpSpeed, 0});
				//m_IsGrounded = false;
			}
			break;
		}
		case Key::E: {
			break;
		}

		default:
			break;
	}
}
void Player::OnKeyReleased(const KeyReleasedEvent& event) {
	switch (event.GetKeyCode()) {
		case Key::Space: {
			break;
		}

		default:
			break;
	}
}



void Player::OnMouseButtonPressed(const MouseButtonPressedEvent& event) {
	//CONSOLE_DEBUG("button pressed");

	// use weapon
	//
	if (!m_waiting_animation) {
		m_WeaponScript->UseWeapon();
	}
// 	glm::vec3 mouse_dir = glm::normalize(m_MouseWorldPosition-Get<Component::Transform>().GlobalPosition);
// 	Get<Component::RigidBody>().ApplyForce(mouse_dir*2500.0f);
	// ((Weapon*)weapon.GetScriptInstance())->UseWeapon(dir);

}
void Player::OnMouseButtonReleased(const MouseButtonReleasedEvent& event) {
	//CONSOLE_DEBUG("button released");
}

void Player::OnMouseScrolled(const MouseScrolledEvent& event) {
	//CONSOLE_DEBUG("scrolled");
}

void Player::RestartGame() {
	Get<Component::RigidBody>().SetPosition({ 0.0f, 6.0f, 0.0f });
	Get<Component::RigidBody>().SetLinearVelocity({ 0.0f, 0.0f, 0.0f });
	m_Health = PLAYER_HEALTH;
	GetGUI()->SetHealth(m_Health);
	GetGlobals()->RestartGame();
}



}  // namespace Enik
