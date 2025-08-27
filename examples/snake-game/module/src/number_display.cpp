#include "number_display.h"


namespace Enik {


void NumberDisplay::SetNumber(int value) {
	if (value < 0 or value > 999999) {
		CONSOLE_DEBUGS_ERROR("NumberDisplay::SetNumber Invalid value {}", std::to_string(value));
		return;
	}
	m_value = value;
	UpdateNumber();
}

void NumberDisplay::OnCreate() {
	UpdateNumber();
}

void NumberDisplay::UpdateNumber() {
	if (m_value < 0) {
		return;
	}


	// get digit values
	std::vector<int> digits;
	int number = m_value;
	int digit;
	do {
		digit = number % 10;
		digits.push_back(digit);
		number /= 10;
	} while (number != 0);


	if (m_Digits.size() < digits.size()) {
		size_t len = (digits.size() - m_Digits.size());
		for (size_t i = 0; i < len; i++) {
			CreateNewDigit();
		}
	}


	if (digits.size() > m_Digits.size()) {
		CONSOLE_DEBUG_ERROR("NumberDisplay unreachable");
		return;
	}

	for (size_t i = 0; i < m_Digits.size(); i++) {
		if (Entity dig = FindEntityByUUID(m_Digits[i])) {
			if (not dig.Has<Component::NativeScript>()) {
				CONSOLE_DEBUGS_ERROR("NumberDisplay digit does not have NativeScript {}", dig.GetTag());
				return;
			}

			dig.Get<Component::Transform>().LocalPosition.x = i * m_Offset;
			dig.Get<Component::Transform>().LocalPosition.z = 0.8f;

			Digit* script = (Digit*)dig.GetScriptInstance();
			if (not script) {
				return;
			}
			script->SetDigit(digits[digits.size() - i - 1]);
		}
	}


	// close last unneeded digits
	if (digits.size() < m_Digits.size()) {
		for (size_t i = 0; i < (m_Digits.size() - digits.size()); i++) {
			if (Entity dig = FindEntityByUUID(m_Digits[m_Digits.size() - i - 1])){
				dig.Get<Component::Transform>().LocalPosition.z = -2.0f;
			}
		}
	}


}

void NumberDisplay::CreateNewDigit() {
	if (m_DigitPrefab.empty()) {
		CONSOLE_DEBUG_ERROR("NumberDisplay::CreateNewDigit m_DigitPrefab is invalid!");
		return;
	}

	Entity new_digit = InstantiatePrefab(m_DigitPrefab);
	new_digit.Reparent(m_Entity);

	m_Digits.push_back(new_digit.GetID());
}

}