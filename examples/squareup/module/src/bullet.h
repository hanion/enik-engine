#pragma once
#include "../api/enik.h"



namespace Enik {


class Bullet : public ScriptableEntity {
public:
	virtual void OnUpdate(Timestep ts) override final;

	virtual void OnCollisionEnter(Entity& other) override final;

	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		return std::vector<NativeScriptField>{
			{ "Hit Damage", FieldType::FLOAT, &HitDamage },
			{ "Lifetime", FieldType::FLOAT, &m_Lifetime },
			{ "shooter", FieldType::ENTITY, &Shooter },
		};
	}

public:
	float HitDamage = 10.0f;
	UUID Shooter = 0;
	glm::vec2 ForwardVector = {0,0};
	glm::vec3 BulletStartPos = {0,0,0};

	float m_Timer = 0.0f;
	float m_Lifetime = 5.0f;

	uint8_t m_pcounter = 0;
};

}