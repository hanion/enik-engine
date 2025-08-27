#include "globals.h"
#include "follow.h"
#include "player.h"

namespace Enik {

void Globals::OnCreate() {
	levels.emplace_back("assets/scenes/main.escn");
	levels.emplace_back("assets/scenes/0.escn");

	Entity cam = FindEntityByName("camera");
	if (cam && cam.Has<Component::NativeScript>()) {
		if (Follow* f = ((Follow*)cam.GetScriptInstance())) {
			f->m_TargetID = Player;
		}
	}
}

void Globals::FinishLevel() {
	level_idx++;
	if (level_idx >= levels.size()) {
		return;
	}

	if (level_idx != levels.size()-1) {
		Get<Component::AudioSources>().Play("finish_level");
	} else {
		Get<Component::AudioSources>().Play("finish_game");
	}

	ChangeScene(levels[level_idx]);

	// re set positions of player, camera
	Entity player = FindEntityByUUID(Player);
	if (player) {
		player.Get<Component::Transform>().LocalPosition = { 0.0f, 1.0f, 0.0f };
		// NOTE: final level
		if (level_idx == levels.size()-1) {
			waiting_animation = true;
			player.Get<Component::Transform>().LocalPosition = { 0.0f, 1.0f, -10.0f };
			player.Get<Component::RigidBody>().SetLinearVelocity({ 0.0f, 0.0f, 0.0f });
			//player.Get<Component::RigidBody>().Force = { 0.0f, 0.0f, 0.0f };
		}
	}
	Entity camera = FindEntityByName("camera");
	if (camera) {
		camera.Get<Component::Transform>().LocalPosition = { 0.0f, 20.0f, 0.0f };
	}
}

void Globals::RestartGame() {
	level_idx = 0;
	ChangeScene(DEAD_SCENE);
	Entity cam = FindEntityByName("camera");
	if (cam) {
		cam.Get<Component::Transform>().LocalPosition = { 0.0f, 6.0f, 0.0f };
	}
	dead = true;
}

}
