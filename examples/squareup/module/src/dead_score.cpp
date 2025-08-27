#include "../api/enik.h"
#include "globals.h"

namespace Enik {

class DeadScore : public ScriptableEntity {
public:
	virtual void OnCreate() override {
		if (Globals* g = GetGlobals()) {
			int sc = g->score;
			if (sc > 0) {
				Get<Component::Text>().Data = std::to_string(sc);
			} else {
				Get<Component::Text>().Data = " 0";
			}
			if (Entity ge = Enik::FindEntityByName("GLOBALS")) {
				DestroyEntity(ge);
			}
		}
	}
};

}

