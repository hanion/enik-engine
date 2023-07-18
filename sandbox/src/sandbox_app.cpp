#include <Enik.h>

class ExampleLayer : public Enik::Layer {
private:
	int m_DebugKeyCode = Enik::Key::Comma;
public:
	ExampleLayer()
		: Layer("Example") {
		
	}
	

	void OnUpdate() override {
		//EN_INFO("ExampleLayer::OnUpdate");
	}


	void OnEvent(Enik::Event& event) override {
		//EN_TRACE("exl {0}", event);

		if (event.IsInCategory(Enik::EventCategory::EventCategoryKeyboard) and event.GetEventType() == Enik::EventType::KeyPressed) {

			if (Enik::Input::IsKeyPressed(m_DebugKeyCode)) {
				EN_WARN("exl     pressed {0}", m_DebugKeyCode);
				m_DebugKeyCode++;
			}
			else {
				if (Enik::Input::IsKeyPressed(Enik::Key::Space)) {
					m_DebugKeyCode++;
				}
				else {
					EN_ERROR("exl not pressed {0}, instead pressed {1}", m_DebugKeyCode, ((Enik::KeyPressedEvent&)event).GetKeyCode());
				}
			}


		}
		else if (event.IsInCategory(Enik::EventCategory::EventCategoryMouseButton) and event.GetEventType() == Enik::EventType::MouseButtonPressed) {
			Enik::MouseCode mc = ((Enik::MouseButtonPressedEvent&)event).GetMouseButton();
			EN_TRACE("exl mb {0}", (mc==0) ? "left" : "right" );
		}
		

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
