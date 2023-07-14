#include <Enik.h>

class ExampleLayer : public Enik::Layer {
private:
	
public:
	ExampleLayer()
		: Layer("Example") {
		
	}
	

	void OnUpdate() override {
		//EN_INFO("ExampleLayer::OnUpdate");
	}


	void OnEvent(Enik::Event& event) override {
		EN_TRACE("ExampleLayer::OnEvent {0}", event);
	}


};




class Sandbox : public Enik::Application {
public:
	Sandbox(){
		EN_TRACE("Sandbox Created");
		PushLayer(new ExampleLayer());
		PushOverlay(new Enik::ImGuiLayer());
	}

	~Sandbox(){
		EN_TRACE("Sandbox Deleted");
	}

	// void Run() override {
	// 	EN_TRACE("Sandbox Run");
	// }
};


Enik::Application* Enik::CreateApplication(){
	return new Sandbox();
}
