#pragma once
#include "../api/enik.h"
#include "weapon.h"

namespace Enik {


class Character : public ScriptableEntity {
public:
	virtual void DamageCharacter(float damage, UUID shooter);

public:
	// NOTE: call from FixedUpdate
	void Move(glm::vec3 input);
	void Jump();

	void FindWeaponScript();

	void RotateWeapon(const glm::vec3& target);

	virtual void OnCollisionEnter(Entity& other) override;
	virtual void OnCollisionExit (Entity& other) override;

	virtual std::vector<NativeScriptField> OnEditorGetFields() override {
		return std::vector<NativeScriptField>{
			{ "Health",      FieldType::FLOAT,  &m_Health },
			{ "Weapon",      FieldType::ENTITY, &m_WeaponUUID },
			{ "Sprite",      FieldType::ENTITY, &m_Sprite },
			{ "Move Speed",  FieldType::FLOAT,  &m_MoveSpeed },
			{ "Accel Air",   FieldType::FLOAT,  &m_AccelAir },
			{ "Accel Ground",FieldType::FLOAT,  &m_AccelGround },
			{ "MaxVelocity", FieldType::FLOAT,  &m_MaxVelocity },
			{ "Jump Force",  FieldType::FLOAT,  &m_JumpForce },
			{ "Jump In Air", FieldType::INT,    &m_MaxAirJump },
		};
	}

public:
	float m_Health = 100.0f;
	
	float m_MoveSpeed = 20.0f;
	float m_AccelAir = 0.1f;
	float m_AccelGround = 0.3f;
	float m_JumpForce = 20.0f;
	float m_MaxVelocity = 30.0f;

	bool m_IsGrounded = false;
	uint64_t m_ground_id = 0;

	int m_MaxAirJump = 1;
	int m_JumpCount = 0;

	Weapon* m_WeaponScript;
	UUID m_WeaponUUID = 0;
	UUID m_Sprite = 0;


};

}
