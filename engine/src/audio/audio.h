#pragma once
#include <base.h>


namespace Enik {

struct AudioEngine;

class Audio {
public:
	static void Init();

	static void Play(const char* filepath);
	static void Play(const std::filesystem::path& filepath);

	~Audio();

private:
	static Scope<Audio> s_Instance;
	Scope<AudioEngine> m_AudioEngine;
};

}