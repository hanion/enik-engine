#include <Enik.h>


class Sandbox : public Enik::Application {
public:
	Sandbox(){
		Print("Sandbox Created");
	}

	~Sandbox(){
		Print("Sandbox Deleted");
	}

};


Enik::Application* Enik::CreateApplication(){
	return new Sandbox();
}
