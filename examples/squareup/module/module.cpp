#include "api/enik.h"

#include "src/player.h"
#include "src/follow.h"
#include "src/weapon.h"
#include "src/bullet.h"
#include "src/enemy.h"
#include "src/globals.h"
#include "src/menu.h"
#include "src/spawner.cpp"
#include "src/gui.h"
#include "src/dead_score.cpp"

#define REGISTER(name) Enik::ScriptRegistry::RegisterScriptClass(#name, []() -> Enik::ScriptableEntity* { return new Enik::name(); });



extern "C" void RegisterAllScripts() {
	REGISTER(Follow);
	REGISTER(Player);
	REGISTER(Weapon);
	REGISTER(Bullet);
	REGISTER(Enemy);
	REGISTER(EnemySpawner);
	REGISTER(GUI);
	REGISTER(DeadScore);
	REGISTER(Globals);
	REGISTER(MainMenu);
}












/*
#include <dlfcn.h>
const char* get_name() {
	Dl_info dl_info;
	if (dladdr((void*)get_name, &dl_info)) {
		return dl_info.dli_fname;
	}
	else {
		return nullptr;
	}
}

void __attribute__((constructor)) construct_script_module() {
	std::cout << "    constructor                      " << get_name() << std::endl;
}

void __attribute__((destructor)) destruct_script_module() {
    std::cout << "    destructor                       " << get_name() << std::endl;
}
*/


