#pragma once
#include <base.h>
#include "core/application.h"

extern Enik::Application* Enik::CreateApplication();

int main(int argc, char** argv) {
	Enik::Log::Init();
	EN_CORE_INFO("	>>> Enik Engine <<< ");
	EN_CORE_INFO("	>>>   Started   <<< ");

	auto app = Enik::CreateApplication();
	app->Run();
	delete app;
}
