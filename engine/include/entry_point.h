#pragma once
#include <Enik.h>

extern Enik::Application* Enik::CreateApplication();

int main(){
	auto app = Enik::CreateApplication();
	app->Run();
	delete app;
}

