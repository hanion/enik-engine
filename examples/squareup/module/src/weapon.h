#pragma once
#include "../api/enik.h"



namespace Enik {


class Weapon : public ScriptableEntity {
public:
	void SetupWeapon(UUID owner) { m_Owner = owner; }
	virtual void UseWeapon();

	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		return std::vector<NativeScriptField>{
			{ "Bullet Prefab", FieldType::PREFAB, &m_BulletPrefabPath },
			{ "Hit Damage", FieldType::FLOAT, &HitDamage },
			{ "owner", FieldType::ENTITY, &m_Owner },
		};
	}

protected:
	Entity InstantiateBullet();

public:
	std::string m_BulletPrefabPath = "";
	UUID m_Owner = 0;

	float HitDamage = 10.0f;
	float BulletSpeed = 20.0f;

	size_t AmmoLeft = 30;

};

}