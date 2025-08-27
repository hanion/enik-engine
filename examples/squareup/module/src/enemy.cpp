#include "enemy.h"
#include "globals.h"
namespace Enik {


float RandomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void Enemy::OnCreate() {
	FindWeaponScript();
	m_PlayerUUID = GetGlobals()->Player;

	srand((unsigned int)time(nullptr));

	for (Entity& sp : GetChildren()) {
		if (sp.Has<Component::SpriteRenderer>()) {
			auto& sprite = sp.Get<Component::SpriteRenderer>();
			sprite.SubTexture->TileIndex.y = (int)RandomFloat(0, 12);
			sprite.UpdateSubTexture();
			break;
		}
	}

	if (Entity weapon = FindEntityByUUID(m_WeaponUUID)) {
		for (Entity& sp : weapon.GetChildren()) {
			if (sp.Has<Component::SpriteRenderer>()) {
				auto& sprite = sp.Get<Component::SpriteRenderer>();
				sprite.SubTexture->TileIndex.x = (int)RandomFloat(37, 42);
				sprite.UpdateSubTexture();
				break;
			}
		}
	}
}

void Enemy::OnUpdate(Timestep ts) {
	if (Get<Component::Transform>().GlobalPosition.y < -25.0f) {
		DestroyEntity(m_Entity);
		return;
	}


	bool wait = GetGlobals()->waiting_animation;
	if (m_StateEnemy == WAITING_ANIMATION) {
		if (!wait) {
			m_StateEnemy = CHASE;
		}
	} else {
		if (wait) {
			m_StateEnemy = WAITING_ANIMATION;
		} else {
			m_StateEnemy = CHASE;
		}
	}

	m_CooldownTimer += ts.GetSeconds();
}


void Enemy::OnFixedUpdate() {
	if (m_StateEnemy == WAITING_ANIMATION) {
		return;
	}

	m_Player = FindEntityByUUID(m_PlayerUUID);
	if (!m_Player) {
		return;
	}

	if (m_StateEnemy == CHASE) {
		Chase();
	}
}

void Enemy::Chase() {
	auto& enemy_transform = Get<Component::Transform>();
	auto& player_transform = m_Player.Get<Component::Transform>();

	glm::vec3 to_player = player_transform.GlobalPosition - enemy_transform.GlobalPosition;
	RotateWeapon(player_transform.GlobalPosition);

	glm::vec2 to_player_2d = glm::vec2(to_player.x, to_player.y);
	float distance = glm::length(to_player_2d);

	if (distance < m_ShootingDistance) {
		if (m_CooldownTimer >= m_Cooldown) {
			m_CooldownTimer = 0.0f;
			if (m_WeaponScript) {
				m_WeaponScript->UseWeapon();
			}
		}

	}

	// Move toward player
	to_player.z = 0.0f;
	to_player = glm::normalize(to_player);
	Move(to_player);
}

}
