#pragma once
#include "../api/enik.h"
#include "button_container.h"

namespace Enik {
class MainMenu : public ButtonContainer {
protected:
	virtual void OnCreate() override;
	virtual void OnPressed(Entity button) override;
};
}
