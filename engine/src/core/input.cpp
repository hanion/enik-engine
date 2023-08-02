#include "core/input.h"
#include "application.h"


namespace Enik {

Scope<Input> Input::s_Instance = CreateScope<Input>();

bool Input::IsKeyPressed(int keycode) {

	GLFWwindow* window = (Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetKey(window, keycode);
	return ((state == GLFW_PRESS) || (state == GLFW_REPEAT));

}

bool Input::IsMouseButtonPressed(int button) {
	GLFWwindow* window = (Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetMouseButton(window, button);
	return (state == GLFW_PRESS);
}

std::pair<float, float> Input::GetMousePosition() {
	GLFWwindow* window = (Application::Get().GetWindow().GetNativeWindow());
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	return { (float)mouseX, (float)mouseY };
}

}