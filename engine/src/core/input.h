#pragma once

#include <base.h>

namespace Enik {

class Input {
	
public:
	static bool IsKeyPressed(int keycode);
	static bool IsMouseButtonPressed(int button);
	static std::pair<float, float> GetMousePosition();

private:
	static Input* s_Instance;
};

}