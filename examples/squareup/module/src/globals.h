#pragma once
#include "../api/enik.h"
#include "common.h"
//#include "script_system/extern_functions.h"



namespace Enik {


class Globals : public ScriptableEntity {
public:
	void FinishLevel();

	virtual void OnCreate() override;

	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		return std::vector<NativeScriptField>{
			{ "Player", FieldType::ENTITY, &Player },
			{ "WaitingAnim", FieldType::BOOL, &waiting_animation },
			{ "Arena", FieldType::STRING, &arena_scene },
		};
	}

	void RestartGame();

public:
	UUID Player;
	std::string arena_scene = ARENA_SCENE;

	bool waiting_animation = false;
	bool dead = false;

	size_t score = 0;

	size_t level_idx = 0;
	std::vector<std::string> levels = {};
};


inline Globals* GetGlobals() {
	Entity g = Enik::FindEntityByName("GLOBALS");
	if (g) { return (Globals*)(g.GetScriptInstance()); }
	return nullptr;
}

}
