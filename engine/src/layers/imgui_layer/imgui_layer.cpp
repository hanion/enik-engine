#include "imgui_layer.h"

#include <base.h>


#include "project/project.h"
#include "core/application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_themes.h"
#include <imgui/imgui_internal.h>

namespace Enik {

ImGuiLayer::ImGuiLayer()
	: Layer("imgui layer") {
}

ImGuiLayer::~ImGuiLayer() {
}


void ImGuiLayer::OnAttach() {
	EN_PROFILE_SCOPE;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	std::string font_0 = Project::FindAssetPath("fonts/Noto_Sans/NotoSans-Regular.ttf");
	if (!font_0.empty()) {
		io.FontDefault = io.Fonts->AddFontFromFileTTF(font_0.c_str(), 16.0f);
	}

	std::string font_1 = Project::FindAssetPath("fonts/Noto_Sans_Mono/NotoSansMono-VariableFont_wdth,wght.ttf");
	if (!font_1.empty()) {
		io.Fonts->AddFontFromFileTTF(font_1.c_str(), 20.0f);
	}

	io.IniFilename = NULL;
	io.LogFilename = NULL;

	ImGui::StyleColorsDark();
	ImGuiTheme::Gray();

	Application& app = Application::Get();
	GLFWwindow* window = app.GetWindow().GetNativeWindow();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");
}


void ImGuiLayer::OnDetach() {
	EN_PROFILE_SCOPE;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


void ImGuiLayer::OnEvent(Event& e) {
	if (m_BlockEvents) {
		return; // ? can not use shortcuts if we block events, so not blocking for now
		ImGuiIO& io = ImGui::GetIO();
		e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
		e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
	}
}

void ImGuiLayer::Begin() {
	EN_PROFILE_SCOPE;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


void ImGuiLayer::End() {
	EN_PROFILE_SCOPE;

	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::Get();
	io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

uint32_t ImGuiLayer::GetActiveWidgetID() {
	return GImGui->ActiveId;
}
}