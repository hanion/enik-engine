#pragma once
#include <base.h>
#include "miniaudio.h"


namespace Enik {

class Audio {
public:
	static void Init();

	static void Play(const char* filepath);
	static void Play(const std::filesystem::path& filepath);

	~Audio();

private:
	static Scope<Audio> s_Instance;
	ma_engine m_AudioEngine;
};

}