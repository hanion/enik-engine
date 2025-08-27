#include "character.h"
#include "player.h"

namespace Enik {

void Character::FindWeaponScript() {
	Entity weapon = FindEntityByUUID(m_WeaponUUID);
	if (not weapon or not weapon.Has<Component::Transform>()) {
		CONSOLE_DEBUG_ERROR("Player: Couldn't find weapon !");
		return;
	}

	m_WeaponScript = (Weapon*)weapon.GetScriptInstance();
	m_WeaponScript->SetupWeapon(GetID());
}


void Character::Move(glm::vec3 input) {
	auto& rb = Get<Component::RigidBody>();
	glm::vec3 vel = rb.GetLinearVelocity();

	input.x *= m_MoveSpeed;

	float accel = m_IsGrounded ? m_AccelGround : m_AccelAir;
	vel.x = glm::mix(vel.x, input.x, accel);
	vel.x = glm::clamp(vel.x, -m_MaxVelocity, m_MaxVelocity);

	rb.SetLinearVelocity(vel);
	rb.ActivateBody();

// 	auto& rb = Get<Component::RigidBody>();
// 	rb.AddForce(input*10.0f);
// 
// 	const auto& rbv = rb.GetLinearVelocity();
// 	if (m_IsGrounded) {
// 		glm::vec3 decelerationForce = -rbv * m_Decelaration;
// 		rb.AddForce(decelerationForce);
// 	}
// 
// 	if (std::abs(rbv.x) > m_MaxVelocity) {
// 		auto new_x = glm::sign(rbv.x) * m_MaxVelocity;
// 		rb.SetLinearVelocity({new_x, rbv.y, rbv.z});
// 	}
// 
// 	m_IsAtFinish = false;
}

void Character::Jump() {
	if (m_IsGrounded || m_JumpCount < m_MaxAirJump) {
		auto& rb = Get<Component::RigidBody>();

		glm::vec3 vel = rb.GetLinearVelocity();
		vel.y = 0.0f;
		rb.SetLinearVelocity(vel);

		rb.AddImpulse({0.0f, m_JumpForce, 0.0f});

		if (!m_IsGrounded) {
			m_JumpCount++;
		}

		m_IsGrounded = false;
	}
}


void Character::RotateWeapon(const glm::vec3& target) {
	// look at
	Entity weapon = FindEntityByUUID(m_WeaponUUID);
	if(!weapon) {
		return;
	}
	Component::Transform& weapon_transform = weapon.Get<Component::Transform>();
	Component::Transform& parent_transform = Get<Component::Transform>();
	glm::vec3 diff = target - weapon_transform.GlobalPosition;

	float rotation_rad = std::atan2(diff.y, diff.x);
	glm::quat world_rotation = glm::angleAxis(rotation_rad, glm::vec3(0, 0, 1));
	glm::quat local_rotation = glm::inverse(parent_transform.GlobalRotation) * world_rotation;
	weapon_transform.LocalRotation = local_rotation;

	//Component::Transform& sprite = FindEntityByUUID(m_Sprite).Get<Component::Transform>();
	//Component::Transform& weapon_sprite = weapon.GetChildren()[0].Get<Component::Transform>();

	// Flip if aiming backwards
	float rotation_deg = glm::degrees(rotation_rad);
	if (rotation_deg > 90.0f || rotation_deg < -90.0f) {
		//sprite.LocalScale.x = -1;
		weapon_transform.LocalScale.y = -1;
	} else {
		//sprite.LocalScale.x = 1;
		weapon_transform.LocalScale.y = 1;
	}
}


void Character::DamageCharacter(float damage, UUID shooter) {
	m_Health -= damage;
	if (Has<Component::AudioSources>()) {
		Get<Component::AudioSources>().Play("hit");
	}
	if (m_Health <= 0.0f) {
		// TODO: do not destroy player
		Entity p = FindEntityByUUID(shooter);
		if (p && p.Has<Component::NativeScript>()) {
			if (ScriptableEntity* si = p.GetScriptInstance()) {
				if (Player* ps = (Player*)si) {
					ps->OnKilledEnemy();
				}
			}
		}
		DestroyEntity(m_Entity);
	}
}


void Character::OnCollisionEnter(Entity& other) {
	if (not other or !other.Has<Component::Tag>()) {
		return;
	}

	if (other.Has<Component::CollisionBody>()) {
		if (other.Get<Component::CollisionBody>().Layer == 0) {
			m_ground_id = other.GetID();
			m_IsGrounded = true;
			m_JumpCount = 0;
		}
	}

}

void Character::OnCollisionExit(Entity& e) {
	if (e.Has<Component::CollisionBody>()) {
		if (e.Get<Component::CollisionBody>().Layer == 0) {
			if (m_ground_id == e.GetID()) {
				m_IsGrounded = false;
			}
		}
	}
}


}
