#pragma once
#include "../api/enik.h"


namespace Enik {

class ButtonContainer : public ScriptableEntity {
protected:
	virtual void OnPressed(Entity button) {};

	virtual void OnUpdate(Timestep ts) override;


	virtual void OnMouseButtonPressed (const MouseButtonPressedEvent&  event) override;
	virtual void OnMouseButtonReleased(const MouseButtonReleasedEvent& event) override;


	virtual std::vector<NativeScriptField> OnEditorGetFields() override {
		return std::vector<NativeScriptField>{
			{ "Camera",       FieldType::ENTITY, &m_CameraUUID }
		};
	}


protected:
	UUID m_CameraUUID;

	Entity* m_OldHoveredButton;
	Entity* m_OldClickedButton;
	Entity* m_HoveredButton;
	Entity* m_ClickedButton;
	Entity* m_SelectedButton;


};

}