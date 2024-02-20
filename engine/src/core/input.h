#pragma once

#include <base.h>

namespace Enik {

class EN_API Input {
public:
	static bool IsKeyPressed(int keycode);
	static bool IsMouseButtonPressed(int button);
	static std::pair<float, float> GetMousePosition();

private:
	static Scope<Input> s_Instance;
};

}