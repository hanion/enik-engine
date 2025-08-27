#pragma once
#include "../api/enik.h"

namespace Enik {


enum class Primary {
	NONE = 0,
	PISTOL, RIFLE, SNIPER
};

enum class Secondary {
	NONE = 0,
	BOMB, STUN_BOMB
};

#define PLAYER_HEALTH 60
#define ARENA_SCENE "assets/scenes/arena.escn"
#define MENU_SCENE "assets/scenes/menu.escn"
#define DEAD_SCENE "assets/scenes/dead.escn"



}
