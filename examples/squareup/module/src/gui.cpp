#include "gui.h"

namespace Enik {

void GUI::OnCreate() {
}

void GUI::OnUpdate(Timestep t) {
}

void GUI::SetHealth(float hp) {
	int visible_hearts = static_cast<int>(std::ceil(hp / 10.0f));

	for (int i = 0; i < 3; ++i) {
		auto& heart = FindEntityByUUID(m_Hearts[i]).Get<Component::SpriteRenderer>();
		heart.Color.a = (i < visible_hearts) ? 1.0f : 0.0f;
	}
}

void GUI::SetScore(int sc) {
	Entity score = FindEntityByUUID(m_Score);
	if (score) {
		if (sc > 0) {
			score.Get<Component::Text>().Data = std::to_string(sc);
		} else {
			score.Get<Component::Text>().Data.clear();
		}
	}
}

}

