#pragma once
#include "../api/enik.h"

namespace Enik {

/*

class Player : public ScriptableEntity {

	virtual void OnCreate() override final;
	virtual void OnUpdate(Timestep ts) override final;
	virtual void OnFixedUpdate() override final;

	virtual void OnKeyPressed         (const KeyPressedEvent&          event) override final;
	virtual void OnKeyReleased        (const KeyReleasedEvent&         event) override final;
	virtual void OnMouseButtonPressed (const MouseButtonPressedEvent&  event) override final;
	virtual void OnMouseButtonReleased(const MouseButtonReleasedEvent& event) override final;
	virtual void OnMouseScrolled      (const MouseScrolledEvent&       event) override final;


	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		return std::vector<NativeScriptField>{
			{ "Camera",      FieldType::ENTITY, &m_CameraUUID },
			{ "Is Grounded", FieldType::BOOL,   &m_IsGrounded },
		};
	}

public:
	UUID m_CameraUUID;
	bool m_IsGrounded;
};

*/

}