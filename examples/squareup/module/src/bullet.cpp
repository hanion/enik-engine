#include "bullet.h"
#include "character.h"
#include "enemy.h"
#include "player.h"
#include <string>




void Enik::Bullet::OnUpdate(Timestep ts) {
	// NOTE: waiting for the physics object to be initialized
	// FIX: maybe add awake?
	if (m_pcounter < 2) {
		m_pcounter++;
	} else if (m_pcounter == 2) {
		m_pcounter = 3;
		Component::RigidBody& bullet_rb = GetOrAdd<Component::RigidBody>();
		bullet_rb.SetPosition(BulletStartPos);
		bullet_rb.AddForce(glm::vec3(ForwardVector, 0) * 1000.0f);
		bullet_rb.ActivateBody();
	}

	m_Timer += ts.GetSeconds();
	if (m_Timer > m_Lifetime) {
		DestroyEntity(m_Entity);
		return;
	}
}

void Enik::Bullet::OnCollisionEnter(Entity& other) {
	if (not other or !other.Has<Component::ID>()) {
		return;
	}
	if (not Shooter) {
		return;
	}
	if (other.GetID() == Shooter) {
		return;
	}

	if (other.GetTag() == "finish") {
		return;
	}


	if (other.GetTag() == "ball" or
		other.GetTag() == "enemy" or
		other.GetTag() == "player"
		) {

		if (other.Has<Component::NativeScript>()) {
			Character* script = dynamic_cast<Character*>(other.GetScriptInstance());
			if (script) {
				script->DamageCharacter(HitDamage, Shooter);
			}
		}
	}

	//CONSOLE_DEBUGS("bullet dead {}", other.GetTag());
	DestroyEntity(m_Entity);
}
