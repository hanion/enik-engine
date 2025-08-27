#include "digit.h"


namespace Enik {


void Digit::SetDigit(int value) {
	if (value < 0 or value > 9) {
		CONSOLE_DEBUGS_ERROR("Digit::SetDigit Invalid value {}", std::to_string(value));
		return;
	}
	m_value = value;
	UpdateDigit();
}

void Digit::UpdateDigit() {
	for (int i = 0; i < 10; i++) {
		if (Entity ent = FindEntityByUUID(m_entities[i])) {
			ent.Get<Component::SpriteRenderer>().Color.a = 0.0f;
			if (i == m_value) {
				ent.Get<Component::SpriteRenderer>().Color.a = 1.0f;
			}
		}
	}
}


}