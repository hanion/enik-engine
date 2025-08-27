#pragma once
#include "../api/enik.h"
#include "character.h"
#include "weapon.h"

namespace Enik {

class Enemy : public Character {
protected:
	virtual void OnCreate() override;
	virtual void OnUpdate(Timestep ts) override;
	virtual void OnFixedUpdate() override;

	virtual std::vector<NativeScriptField> OnEditorGetFields() override {
		auto fields = Character::OnEditorGetFields();
		fields.emplace_back("Shoot Dist.", FieldType::FLOAT, &m_ShootingDistance);
		fields.emplace_back("Cooldown", FieldType::FLOAT, &m_Cooldown);
		fields.emplace_back("Bullet Speed", FieldType::FLOAT, &m_BulletSpeedMultiplier);
		fields.emplace_back("EnemyType", FieldType::INT, &Type);
		fields.emplace_back("State", FieldType::INT, &m_StateEnemy);
		return fields;
	}

	virtual void Chase();

public:
	UUID m_PlayerUUID = 0;
	Entity m_Player;

	float m_ShootingDistance = 10.0f;
	float m_BulletSpeedMultiplier = 0.5f;

	float m_Cooldown = 1.0f;
	float m_CooldownTimer = 0.0f;

	enum StateEnemy {
		NONE = 0, CHASE, WAITING_ANIMATION
	};
	StateEnemy m_StateEnemy = CHASE;

	uint8_t Type = 1;
};

}