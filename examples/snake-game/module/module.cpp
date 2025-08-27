#include "api/enik.h"

#include "src/game_manager.h"
#include "src/snake.h"
#include "src/button_container.h"
#include "src/main_menu.h"
#include "src/digit.h"
#include "src/number_display.h"

#define REGISTER(name) Enik::ScriptRegistry::RegisterScriptClass(#name, \
	[]() -> Enik::ScriptableEntity* { return new Enik::name(); });



extern "C" void RegisterAllScripts() {
	REGISTER(GameManager);
	REGISTER(Snake);
	REGISTER(ButtonContainer);
	REGISTER(MainMenu);
	REGISTER(Digit);
	REGISTER(NumberDisplay);
}


