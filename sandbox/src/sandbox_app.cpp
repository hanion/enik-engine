#include <Enik.h>

using namespace Enik;

class ExampleLayer : public Layer {
private:
	int m_DebugKeyCode = Key::Comma;
public:
	ExampleLayer()
		: Layer("Example") {
		
	}
	

	void OnUpdate() override {
		//EN_INFO("ExampleLayer::OnUpdate");
	}


	void OnEvent(Event& event) override {
		//EN_TRACE("exl {0}", event);

		if (event.IsInCategory(EventCategory::EventCategoryKeyboard) and event.GetEventType() == EventType::KeyPressed) {

			if (Input::IsKeyPressed(m_DebugKeyCode)) {
				EN_WARN("exl     pressed {0}", m_DebugKeyCode);
				m_DebugKeyCode++;
			}
			else {
				if (Input::IsKeyPressed(Key::Space)) {
					m_DebugKeyCode++;
				}
				else {
					EN_ERROR("exl not pressed {0}, instead pressed {1}", m_DebugKeyCode, ((KeyPressedEvent&)event).GetKeyCode());
				}
			}


		}
		else if (event.IsInCategory(EventCategory::EventCategoryMouseButton) and event.GetEventType() == EventType::MouseButtonPressed) {
			MouseCode mc = ((MouseButtonPressedEvent&)event).GetMouseButton();
			EN_TRACE("exl mb {0}", (mc==0) ? "left" : "right" );
		}
		

	}


};




class Sandbox : public Application {
public:
	Sandbox(){
		EN_TRACE("Sandbox Created");
		PushLayer(new ExampleLayer());
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
