#include "api/enik.h"

//#include "src/player.h"

#define REGISTER(name) Enik::ScriptRegistry::RegisterScriptClass(#name, \
	[]() -> Enik::ScriptableEntity* { return new Enik::name(); });



extern "C" void RegisterAllScripts() {
	//REGISTER(Player);
}


