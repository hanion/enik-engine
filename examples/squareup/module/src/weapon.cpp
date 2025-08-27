#include "weapon.h"
#include "bullet.h"
#include "globals.h"
#include <string>

namespace Enik {


Entity Weapon::InstantiateBullet() {
	Entity entity = {};
	if (m_Owner == 0) {
		CONSOLE_DEBUG_ERROR("Weapon Owner is not set!");
		return entity;
	}
	if (m_BulletPrefabPath.empty()) {
		return entity;
	}

	Entity bullet = InstantiatePrefab(m_BulletPrefabPath);
	if (!bullet) {
		return entity;
	}

	return bullet;
}

void Weapon::UseWeapon() {
	Entity bullet = InstantiateBullet();
	if (!bullet) {
		return;
	}

	bullet.Reparent(Enik::FindEntityByName("ENTITIES"));

	Component::Transform& weapon_transform = Get<Component::Transform>();
	glm::vec3 forward3 = weapon_transform.GlobalRotation * glm::vec3(1, 0, 0);
	glm::vec2 forward = glm::normalize(glm::vec2(forward3.x, forward3.y))*1.1f;
	auto bullet_pos = weapon_transform.GlobalPosition + glm::vec3(forward, 0.0f);

	Component::RigidBody& bullet_rb = bullet.Get<Component::RigidBody>();
	auto& tr = bullet.Get<Component::Transform>();
	tr.LocalPosition = bullet_pos;
	tr.GlobalScale.x = 0.001f;
	tr.GlobalScale.y = 0.001f;
	bullet_rb.SetPosition(bullet_pos);

	Bullet* script = (Bullet*)bullet.GetScriptInstance();
	if (script) {
		script->Shooter = m_Owner;
		script->HitDamage = HitDamage;
		script->ForwardVector = forward;
		script->BulletStartPos = bullet_pos;
	} else {
		CONSOLE_DEBUG_ERROR("WTF");
	}

	Get<Component::AudioSources>().Play("shoot");
}

}
