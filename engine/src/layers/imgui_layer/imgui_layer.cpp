#include "imgui_layer.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../../application.h"

namespace Enik {

ImGuiLayer::ImGuiLayer() 
	: Layer("imgui layer") {

}

ImGuiLayer::~ImGuiLayer() {

}


void ImGuiLayer::OnAttach() {
  IMGUI_CHECKVERSION();
	ImGui::CreateContext();
  
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


	ImGui::StyleColorsDark();
	
	Application& app = Application::Get();
	GLFWwindow* window = app.GetWindow().GetNativeWindow();

	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");
	
}


void ImGuiLayer::OnDetach() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


void ImGuiLayer::OnImGuiRender() {
	static bool show = true;
	ImGui::ShowDemoWindow(&show);
	ShowTestingWindow();
}



void ImGuiLayer::Begin() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


void ImGuiLayer::End() {
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



void ImGuiLayer::ShowTestingWindow() {



	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoDecoration;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->WorkPos.y + 20), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(300,42));
	if (ImGui::Begin("testing window", NULL, window_flags)) {
		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);



		ImGui::Spacing();
		static float slider_float = 1.0f;
		ImGui::SliderFloat("slider float",&slider_float,0.0f,100.0f, "%.2f");

		

		ImGui::End();
	}
}

}