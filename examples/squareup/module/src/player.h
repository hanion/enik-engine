#pragma once
#include "../api/enik.h"
#include "character.h"
#include "weapon.h"



namespace Enik {


class Player : public Character {
public:
	virtual void DamageCharacter(float damage, UUID shooter) override;
	void OnKilledEnemy();


private:
	virtual void OnCreate() override final;
	virtual void OnUpdate(Timestep ts) override final;
	virtual void OnFixedUpdate() override final;

	virtual void OnKeyPressed (const KeyPressedEvent&  event) override final;
	virtual void OnKeyReleased(const KeyReleasedEvent& event) override final;
	virtual void OnMouseButtonPressed (const MouseButtonPressedEvent&  event) override final;
	virtual void OnMouseButtonReleased(const MouseButtonReleasedEvent& event) override final;
	virtual void OnMouseScrolled(const MouseScrolledEvent& event) override final;



	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		auto fields = Character::OnEditorGetFields();
		fields.emplace_back("Camera", FieldType::ENTITY, &m_CameraUUID);
		fields.emplace_back("is_grounded", FieldType::BOOL, &m_IsGrounded);
		return fields;
	}

	void RestartGame();
	void AddScore(int s);
	void SetScore(int s);

public:
	UUID m_CameraUUID;
	glm::vec3 m_MouseWorldPosition = glm::vec3(0);

	bool m_waiting_animation = false;
	bool died_once = false;

	bool m_JumpPressedLastFrame = false;
};

}