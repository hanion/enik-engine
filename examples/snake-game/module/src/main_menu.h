#pragma once
#include "../api/enik.h"
#include "button_container.h"

namespace Enik {

class MainMenu : public ButtonContainer {
private:
	virtual void OnPressed(Entity button) override final;

	virtual void OnUpdate(Timestep ts) override final;


	virtual void OnMouseButtonPressed (const MouseButtonPressedEvent&  event) override final;
	virtual void OnMouseButtonReleased(const MouseButtonReleasedEvent& event) override final;

	virtual void OnKeyPressed (const KeyPressedEvent&  event) override final;


	void PlayGame(bool is_playing);
	void MoveCam(bool to_game);



	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		auto all_fields = ButtonContainer::OnEditorGetFields();
		all_fields.emplace_back(
			NativeScriptField("Game", FieldType::ENTITY, &m_Game)
		);
		return all_fields;
	}


private:
	UUID m_Game;

};

}