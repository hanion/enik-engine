#include <Enik.h>


class Sandbox : public Enik::Application {
public:
	Sandbox(){
		EN_TRACE("Sandbox Created");
	}

	~Sandbox(){
		EN_TRACE("Sandbox Deleted");
	}

	void Run() override {
		EN_TRACE("Sandbox Run");
	}
};


Enik::Application* Enik::CreateApplication(){
	return new Sandbox();
}
